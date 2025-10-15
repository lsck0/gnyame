#include "nyangine/nyangine.h"

void on_create();
void on_destroy();
void on_event(NYA_Window* window, NYA_Event* event);
void on_update(NYA_Window* window, f32 delta_time);

s32 main(s32 argc, char** argv) {
  nya_unused(argc, argv);

  NYA_App app = nya_app_new((NYA_AppConfig){
      .time_step_ms     = 15,
      .frame_rate_limit = 120,
      .vsync_enabled    = false,
  });

  nya_window_new(&app, "gnyame", 1280, 720, NYA_WINDOW_RESIZABLE, "main_window");
  nya_window_layer_push(
      &app,
      "main_window",
      (NYA_Layer){
          .id         = "main_layer",
          .enabled    = true,
          .on_create  = on_create,
          .on_destroy = on_destroy,
          .on_event   = on_event,
          .on_update  = on_update,
          .on_render  = nullptr,
      }
  );

  nya_app_run(&app);
  nya_app_destroy(&app);

  return 0;
}

void on_create() {
  nya_info("gnyame (v%s) started", NYA_VERSION);
}

void on_update(NYA_Window* window, f32 delta_time) {
  nya_unused(window);
  nya_unused(delta_time);

  nya_info("gnyame updating");
}

void on_event(NYA_Window* window, NYA_Event* event) {
  nya_unused(window);
  nya_unused(event);
}

void on_destroy() {
  nya_info("gnyame shutting down");
}
