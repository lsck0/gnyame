#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"

#include "nyangine/base/arena.h"
#include "nyangine/base/array.h"
#include "nyangine/base/attributes.h"
#include "nyangine/base/string.h"
#include "nyangine/core/event.h"

typedef struct NYA_Layer     NYA_Layer;
typedef struct NYA_Window    NYA_Window;
typedef struct NYA_App       NYA_App;
typedef struct NYA_AppConfig NYA_AppConfig;
nya_derive_array(NYA_Layer);
nya_derive_array(NYA_Window);

struct NYA_AppConfig {
  u32  time_step_ms;
  u32  frame_rate_limit;
  bool vsync_enabled;
};

struct NYA_App {
  NYA_AppConfig config;

  NYA_Arena* global_allocator;
  NYA_Arena* entity_allocator;
  NYA_Arena* frame_allocator;

  SDL_GPUDevice*  gpu_device;
  NYA_WindowArray windows;
};

typedef enum {
  NYA_WINDOW_NONE      = 0,
  NYA_WINDOW_RESIZABLE = SDL_WINDOW_RESIZABLE,
} NYA_WindowFlags;

struct NYA_Window {
  void*       id;
  SDL_Window* sdl_window;

  NYA_LayerArray layer_stack;
};

struct NYA_Layer {
  void* id;
  bool  enabled;

  void (*on_create)(void);
  void (*on_destroy)(void);

  void (*on_event)(NYA_Window* window, NYA_Event* event);
  void (*on_update)(NYA_Window* window, f32 delta_time);
  void (*on_render)(NYA_Window* window);
};

extern NYA_App nya_app_new(NYA_AppConfig config);
extern void    nya_app_destroy(NYA_App* app);
extern void    nya_app_option_vsync_set(NYA_App* app, bool vsync);
extern void    nya_app_run(NYA_App* app);

// clang-format off
extern void*       nya_window_new(NYA_App* app, const char* title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags, void* id);
extern void        nya_window_destroy(NYA_App* app, void* window_id);
extern NYA_Window* nya_window_get(NYA_App* app, void* window_id);
extern void        nya_window_layer_enable(NYA_App* app, void* window_id, void* layer_id);
extern void        nya_window_layer_disable(NYA_App* app, void* window_id, void* layer_id);
extern void        nya_window_layer_push(NYA_App* app, void* window_id, NYA_Layer layer);
extern NYA_Layer   nya_window_layer_pop(NYA_App* app, void* window_id);
// clang-format on
