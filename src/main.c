#include <pebble.h>

static Window* s_main_window;

static void main_load_window(Window *window) {
  
}

static void main_unload_window(Window *window) {
  
}

static void init() {
  
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_load_window,
    .unload = main_unload_window
  });
  
  window_stack_push(s_main_window, true);
}

static void deinit() {
 window_destroy(s_main_window);
}
  
int main(int argc, char **argv) {
  init();
  app_event_loop();
  deinit();
}