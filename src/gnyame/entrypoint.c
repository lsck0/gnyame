#include "gnyame/entrypoint.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>

int entrypoint(int argc, char** argv) {
  (void)argc;
  (void)argv;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_Init(): %s\n", SDL_GetError());
    return -1;
  }

  SDL_Window* window = SDL_CreateWindow("Hello World", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateWindow(): %s\n", SDL_GetError());
    return -1;
  }

  SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,
      true,
      nullptr
  );
  if (gpu_device == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateGPUDevice(): %s\n", SDL_GetError());
    return -1;
  }

  if (!SDL_ClaimWindowForGPUDevice(gpu_device, window)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
    return -1;
  }
  SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

  bool should_quit = false;
  while (!should_quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) should_quit = true;
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
        should_quit = true;
      }
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);

    SDL_GPUTexture* swapchain_texture;
    SDL_AcquireGPUSwapchainTexture(command_buffer, window, &swapchain_texture, nullptr, nullptr);

    if (swapchain_texture != nullptr) {
      SDL_GPUColorTargetInfo target_info = {};
      target_info.texture                = swapchain_texture;
      target_info.clear_color            = (SDL_FColor){0};
      target_info.load_op                = SDL_GPU_LOADOP_CLEAR;
      target_info.store_op               = SDL_GPU_STOREOP_STORE;
      target_info.mip_level              = 0;
      target_info.layer_or_depth_plane   = 0;
      target_info.cycle                  = false;

      SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

      SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(command_buffer);
  }

  SDL_WaitForGPUIdle(gpu_device);

  SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
  SDL_DestroyGPUDevice(gpu_device);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
