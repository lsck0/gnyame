#include "SDL3/SDL_gpu.h"

#include "nyangine/base/assert.h"
#include "nyangine/base/ints.h"
#include "nyangine/core/app.h"

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

void* nya_window_new(
    NYA_App*        app,
    const char*     title,
    u32             initial_width,
    u32             initial_height,
    NYA_WindowFlags flags,
    void*           id
) {
  nya_assert(app);
  nya_assert(title);
  nya_assert(initial_width > 0);
  nya_assert(initial_height > 0);

  static u32 id_counter = 0;

  bool ok;

  SDL_Window* sdl_window = SDL_CreateWindow(title, (s32)initial_width, (s32)initial_height, flags);
  nya_assert(sdl_window != nullptr, "SDL_CreateWindow() failed: %s", SDL_GetError());

  ok = SDL_ClaimWindowForGPUDevice(app->gpu_device, sdl_window);
  nya_assert(ok, "SDL_ClaimWindowForGPUDevice() failed: %s", SDL_GetError());

  ok = SDL_SetGPUSwapchainParameters(
      app->gpu_device,
      sdl_window,
      SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
      app->config.vsync_enabled ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX
  );
  nya_assert(ok, "SDL_SetGPUSwapchainParameters() failed: %s", SDL_GetError());

  NYA_Window nya_window = {
      .id          = id != nullptr ? id : (void*)(uintmax_t)(++id_counter),
      .sdl_window  = sdl_window,
      .layer_stack = nya_array_new(app->global_allocator, NYA_Layer),
  };

  nya_array_push(&app->windows, nya_window);

  return nya_window.id;
}

void nya_window_destroy(NYA_App* app, void* window_id) {
  nya_assert(app);
  nya_assert(window_id);

  NYA_Window* window = nya_window_get(app, window_id);

  nya_array_foreach_reverse (&window->layer_stack, layer) {
    if (layer->on_destroy != nullptr) layer->on_destroy();
  }
  nya_array_destroy(&window->layer_stack);

  SDL_WaitForGPUIdle(app->gpu_device);
  SDL_ReleaseWindowFromGPUDevice(app->gpu_device, window->sdl_window);
  SDL_DestroyWindow(window->sdl_window);
}

NYA_Window* nya_window_get(NYA_App* app, void* window_id) {
  nya_assert(app);
  nya_assert(window_id);

  nya_array_foreach (&app->windows, window) {
    if (window->id == window_id) return window;
  }

  nya_panic("Cannot get window: window with id %p not found", window_id);
}

void nya_window_layer_enable(NYA_App* app, void* window_id, void* layer_id) {
  nya_assert(app);
  nya_assert(window_id);
  nya_assert(layer_id);

  NYA_Window* window = nya_window_get(app, window_id);
  nya_array_foreach (&window->layer_stack, layer) {
    if (layer->id == layer_id) {
      layer->enabled = true;
      return;
    }
  }

  nya_panic("Cannot enable layer: layer with id %p not found", layer_id);
}

void nya_window_layer_disable(NYA_App* app, void* window_id, void* layer_id) {
  nya_assert(app);
  nya_assert(window_id);
  nya_assert(layer_id);

  NYA_Window* window = nya_window_get(app, window_id);
  nya_array_foreach (&window->layer_stack, layer) {
    if (layer->id == layer_id) {
      layer->enabled = false;
      return;
    }
  }

  nya_panic("Cannot disable layer: layer with id %p not found", layer_id);
}

void nya_window_layer_push(NYA_App* app, void* window_id, NYA_Layer layer) {
  nya_assert(window_id);

  NYA_Window* window = nya_window_get(app, window_id);

  if (layer.on_create != nullptr) layer.on_create();

  nya_array_push(&window->layer_stack, layer);
}

NYA_Layer nya_window_layer_pop(NYA_App* app, void* window_id) {
  nya_assert(app);
  nya_assert(window_id);

  NYA_Window* window = nya_window_get(app, window_id);
  nya_assert(window->layer_stack.length > 0, "Cannot pop layer: layer stack is empty");

  NYA_Layer layer = nya_array_last(&window->layer_stack);
  if (layer.on_destroy != nullptr) layer.on_destroy();

  return nya_array_pop(&window->layer_stack);
}
