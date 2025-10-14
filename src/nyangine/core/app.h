#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"

#include "nyangine/base/arena.h"
#include "nyangine/base/array.h"
#include "nyangine/base/attributes.h"
#include "nyangine/base/string.h"
#include "nyangine/core/event.h"

typedef struct NYA_Layer  NYA_Layer;
typedef struct NYA_Window NYA_Window;
typedef struct NYA_App    NYA_App;
nya_derive_array(NYA_Layer);
nya_derive_array(NYA_Window);

struct NYA_Layer {
  void* id;
  bool  enabled;

  void (*on_create)(void);
  void (*on_destroy)(void);

  void (*on_event)(NYA_Window* window, NYA_Event* event);
  void (*on_update)(NYA_Window* window, f32 delta_time);
  void (*on_render)(NYA_Window* window);
};

typedef enum {
  NYA_WINDOW_NONE = 0,
} NYA_WindowFlags;

struct NYA_Window {
  void*          id;
  SDL_Window*    window;
  SDL_GPUDevice* gpu_device;

  NYA_LayerArray layer_stack;
};

struct NYA_App {
  NYA_Arena* global_allocator;
  NYA_Arena* frame_allocator;

  NYA_WindowArray windows;
};

extern NYA_App nya_app_create(s32 argc, char** argv);
extern NYA_App nya_app_destroy(NYA_App* app);
extern void    nya_app_run(NYA_App* app);

// clang-format off
extern void* nya_window_create(NYA_App* app, NYA_String title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags);
extern void  nya_window_destroy(NYA_App* app, void* window_id);
extern void  nya_window_layer_disable(NYA_Window* window, void* layer_id);
extern void  nya_window_layer_enable(NYA_Window* window, void* layer_id);
extern void  nya_window_layer_pop(NYA_Window* window);
extern void  nya_window_layer_push(NYA_Window* window, NYA_Layer layer);
// clang-format on
