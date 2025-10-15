#include "nyangine/core/app.h"

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_timer.h"

#include "nyangine/base/arena.h"
#include "nyangine/base/array.h"
#include "nyangine/base/assert.h"
#include "nyangine/base/memory.h"
#include "nyangine/base/misc.h"
#include "nyangine/base/perf.h"
#include "nyangine/base/version.h"

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_App nya_app_new(NYA_AppConfig config) {
  bool ok = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  nya_assert(ok, "SDL_Init() failed: %s", SDL_GetError());

  SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB | SDL_GPU_SHADERFORMAT_SPIRV,
      NYA_IS_DEBUG,
      nullptr
  );
  nya_assert(gpu_device != nullptr, "SDL_CreateGPUDevice() failed: %s", SDL_GetError());

  NYA_App app = {
      .config           = config,
      .global_allocator = nya_arena_new(),
      .entity_allocator = nya_arena_new(),
      .frame_allocator  = nya_arena_new(),
      .gpu_device       = gpu_device,
      .windows          = nya_array_new(app.global_allocator, NYA_Window),
  };

  return app;
}

void nya_app_destroy(NYA_App* app) {
  nya_assert(app);

  SDL_WaitForGPUIdle(app->gpu_device);
  nya_array_foreach (&app->windows, window) nya_window_destroy(app, window->id);
  nya_array_destroy(&app->windows);
  SDL_DestroyGPUDevice(app->gpu_device);

  nya_arena_destroy(app->global_allocator);
  nya_arena_destroy(app->entity_allocator);
  nya_arena_destroy(app->frame_allocator);
}

void nya_app_option_vsync_set(NYA_App* app, bool vsync) {
  nya_assert(app);

  if (app->config.vsync_enabled == vsync) return;

  app->config.vsync_enabled = vsync;
  nya_array_foreach (&app->windows, window) {
    bool ok = SDL_SetGPUSwapchainParameters(
        app->gpu_device,
        window->sdl_window,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        app->config.vsync_enabled ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX
    );
    nya_assert(ok, "SDL_SetGPUSwapchainParameters() failed: %s", SDL_GetError());
  }
}

void nya_app_run(NYA_App* app) {
  nya_assert(app);

  u64  previous_time_ms = 0;
  s64  time_behind_ms   = 0;
  bool should_quit      = false;

  while (!should_quit) {
    u64 current_time_ms  = SDL_GetTicks();
    u64 elapsed_ms       = current_time_ms - previous_time_ms;
    previous_time_ms     = current_time_ms;
    time_behind_ms      += (s64)elapsed_ms;

    // input and event handling
    {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        // app and window closing
        if (event.type == SDL_EVENT_QUIT) should_quit = true;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
          nya_array_for (&app->windows, window_idx) {
            void*       window_id  = app->windows.items[window_idx].id;
            SDL_Window* sdl_window = app->windows.items[window_idx].sdl_window;

            if (SDL_GetWindowID(sdl_window) == event.window.windowID) {
              nya_window_destroy(app, window_id);
              nya_array_remove(&app->windows, window_idx);
              if (app->windows.length == 0) should_quit = true;
              break;
            }
          }
        }

        // propagate event to layers
        // TODO: event and input system
        nya_array_foreach (&app->windows, window) {
          nya_array_foreach_reverse (&window->layer_stack, layer) {
            if (layer->enabled && layer->on_event != nullptr) {
              NYA_Event nya_event = {0};
              layer->on_event(window, &nya_event);
            }
          }
        }
      }
    }

    // updating
    {
      while (time_behind_ms >= app->config.time_step_ms) {
        nya_array_foreach (&app->windows, window) {
          nya_array_foreach (&window->layer_stack, layer) {
            if (layer->enabled && layer->on_update != nullptr) {
              layer->on_update(window, (f32)(app->config.time_step_ms) / 1000.0F);
            }
          }
        }
        time_behind_ms -= app->config.time_step_ms;
      }
    }

    // rendering
    {
      nya_array_foreach (&app->windows, window) {
        SDL_GPUDevice* gpu_device = app->gpu_device;
        SDL_Window*    sdl_window = window->sdl_window;

        SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);

        SDL_GPUTexture* swapchain_texture;
        SDL_AcquireGPUSwapchainTexture(command_buffer, sdl_window, &swapchain_texture, nullptr, nullptr);

        if (swapchain_texture != nullptr) {
          SDL_GPUColorTargetInfo target_info = {
              .texture     = swapchain_texture,
              .clear_color = (SDL_FColor){0},
              .load_op     = SDL_GPU_LOADOP_CLEAR,
              .store_op    = SDL_GPU_STOREOP_STORE,
          };
          SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);
          nya_assert(render_pass != nullptr, "SDL_BeginGPURenderPass() failed");

          nya_array_foreach (&app->windows, window) {
            nya_array_foreach (&window->layer_stack, layer) {
              if (layer->enabled && layer->on_render != nullptr) layer->on_render(window);
            }
          }

          // TODO: rendering ...
          // render(lag / app->config.time_step_ms);

          SDL_EndGPURenderPass(render_pass);
        }

        SDL_SubmitGPUCommandBuffer(command_buffer);
      }
    }

    // framerate limiting
    if (!app->config.vsync_enabled && app->config.frame_rate_limit > 0) {
      u64 min_frame_time_ms = 1000 / app->config.frame_rate_limit;
      if (elapsed_ms < min_frame_time_ms) SDL_Delay((u32)(min_frame_time_ms - elapsed_ms));
    }
  }
}
