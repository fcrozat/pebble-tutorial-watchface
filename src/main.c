#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

static Window* s_main_window;
static TextLayer* s_time_layer;
static GFont *s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_text_layer;
static TextLayer *s_weather_layer;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    static char temperature_buffer[8];
    static char conditions_buffer[32];
    static char weather_layer_buffer[32];

    Tuple *t = dict_read_first(iterator);
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
    if (t == NULL)
        APP_LOG(APP_LOG_LEVEL_ERROR,"empty iterator");

    while(t != NULL) {
        APP_LOG(APP_LOG_LEVEL_INFO, "key %d",(int)t->key);
        switch(t->key) {
            case KEY_TEMPERATURE:
                snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C",
                        (int)t->value->int32);
                break;
            case KEY_CONDITIONS:
                snprintf(conditions_buffer, sizeof(conditions_buffer), "%s",
                        t->value->cstring);
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        t = dict_read_next(iterator);
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Temperature:%s|",temperature_buffer);
    APP_LOG(APP_LOG_LEVEL_INFO, "Conditions:%s|",conditions_buffer);

    snprintf(weather_layer_buffer,sizeof(weather_layer_buffer),"%s - %s",
            temperature_buffer,conditions_buffer);

    text_layer_set_text (s_weather_layer,weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

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

    s_text_layer = text_layer_create(GRect(0,100,144,30));
    text_layer_set_background_color (s_text_layer, GColorClear);
    text_layer_set_text_color (s_text_layer, GColorBlack);
    text_layer_set_font (s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment (s_text_layer, GTextAlignmentCenter);
    text_layer_set_text (s_text_layer, "#hackweek");
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_layer));

    s_weather_layer = text_layer_create(GRect(0,130,144,30));
    text_layer_set_background_color (s_weather_layer, GColorClear);
    text_layer_set_text_color (s_weather_layer, GColorBlack);
    text_layer_set_font (s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment (s_weather_layer, GTextAlignmentCenter);
    text_layer_set_text (s_weather_layer, "Loading...");

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

    s_time_layer = text_layer_create(GRect(5, 52, 139, 50));

    text_layer_set_background_color (s_time_layer, GColorClear);
    text_layer_set_text_color (s_time_layer, GColorBlack);

    text_layer_set_font (s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment (s_time_layer, GTextAlignmentCenter);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

    s_time_font = fonts_load_custom_font (resource_get_handle (RESOURCE_ID_FONT_PERFECT_DOS_48));
    text_layer_set_font (s_time_layer, s_time_font);



}

static void main_unload_window(Window *window) {


    text_layer_destroy (s_weather_layer);
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

    tick_timer_service_subscribe (MINUTE_UNIT, tick_handler);

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    app_message_open(app_message_inbox_size_maximum(),
                     app_message_outbox_size_maximum());


    update_time();
}

static void deinit() {
    tick_timer_service_unsubscribe ();
    window_destroy(s_main_window);
}

int main(int argc, char **argv) {
    init();
    app_event_loop();
    deinit();
}
