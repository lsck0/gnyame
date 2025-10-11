#include "nyangine/core/app.h"

#include "nyangine/base/assert.h"
#include "nyangine/base/misc.h"

/*
 * ─────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────
 * */

NYA_App nya_app_create(s32 argc, char** argv) {
  nya_unused(argc, argv);

  nya_todo();
}

NYA_App nya_app_destroy(NYA_App* app) {
  nya_unused(app);

  nya_todo();
}

void nya_app_run(NYA_App* app) {
  nya_unused(app);

  nya_todo();
}

void* nya_window_create(NYA_App* app, NYA_String title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags) {
  nya_unused(app, title, initial_width, initial_height, flags);

  nya_todo();
}

void nya_window_destroy(NYA_App* app, void* window_id) {
  nya_unused(app, window_id);

  nya_todo();
}

void nya_window_layer_disable(NYA_Window* window, void* layer_id) {
  nya_unused(window, layer_id);

  nya_todo();
}

void nya_window_layer_enable(NYA_Window* window, void* layer_id) {
  nya_unused(window, layer_id);

  nya_todo();
}

void nya_window_layer_pop(NYA_Window* window) {
  nya_unused(window);

  nya_todo();
}

void nya_window_layer_push(NYA_Window* window, NYA_Layer layer) {
  nya_unused(window, layer);

  nya_todo();
}
