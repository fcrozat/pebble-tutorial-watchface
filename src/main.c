#include <pebble.h>

static Window* s_main_window;
static TextLayer* s_time_layer;
static GFont *s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_text_layer;


static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char buffer[] = "00:00";

    if (clock_is_24h_style() == true) {
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }

    text_layer_set_text (s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_load_window(Window *window) {

    s_background_bitmap = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_GEEKO);
    s_background_layer = bitmap_layer_create(GRect(0,0,144,168));
    bitmap_layer_set_bitmap(s_background_layer,s_background_bitmap);
    bitmap_layer_set_alignment(s_background_layer,GAlignTop);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

    s_text_layer = text_layer_create(GRect(0,100,144,168));
    text_layer_set_background_color (s_text_layer, GColorClear);
    text_layer_set_text_color (s_text_layer, GColorBlack);

    text_layer_set_background_color (s_time_layer, GColorClear);
    text_layer_set_text_color (s_time_layer, GColorBlack);

    text_layer_set_text (s_time_layer, "#hackweek");

    text_layer_set_font (s_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment (s_text_layer, GTextAlignmentCenter);

    s_time_layer = text_layer_create(GRect(5, 52, 139, 50));

    text_layer_set_background_color (s_time_layer, GColorClear);
    text_layer_set_text_color (s_time_layer, GColorBlack);

    text_layer_set_font (s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment (s_time_layer, GTextAlignmentCenter);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

    s_time_font = fonts_load_custom_font (resource_get_handle (RESOURCE_ID_FONT_PERFECT_DOS_48));
    text_layer_set_font (s_time_layer, s_time_font);


    tick_timer_service_subscribe (MINUTE_UNIT, tick_handler);

}

static void main_unload_window(Window *window) {

    tick_timer_service_unsubscribe ();

    text_layer_destroy (s_time_layer);
    text_layer_destroy (s_text_layer);
    fonts_unload_custom_font(s_time_font);

    gbitmap_destroy(s_background_bitmap);
    bitmap_layer_destroy(s_background_layer);
}


static void init() {

    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_load_window,
            .unload = main_unload_window
            });

    window_stack_push(s_main_window, true);

    update_time();
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(int argc, char **argv) {
    init();
    app_event_loop();
    deinit();
}
