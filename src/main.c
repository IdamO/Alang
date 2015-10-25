#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_heading_layer, *s_prompt_layer;
  
static Window *s_splash_window;
static Layer *s_splash_layer;
static BitmapLayer *s_logo_layer;
static GBitmap *s_logo_bitmap;

AppTimer *s_splash_timer;

static DictationSession *s_dictation_session;
static char s_last_text[256];

static bool s_speaking_enabled;

static const uint32_t dictionaryKey = 0xabbababe;

static void next_questions_handler(void *context) {
  s_speaking_enabled = true;
}

static void check_answer() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, dictionaryKey, s_last_text);
  app_message_outbox_send();
  app_timer_register(3000, next_questions_handler, NULL);
  vibes_double_pulse();
}

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context){
  if(status == DictationSessionStatusSuccess) {
    // Check this answer
    strncpy(s_last_text, transcription, sizeof(s_last_text));
    text_layer_set_text(s_prompt_layer, "Hey what's up, hello?");
    check_answer();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Transcription failed.\n\nError ID:\n%d", (int)status);
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    // Start voice dictation UI
    dictation_session_start(s_dictation_session);
    s_speaking_enabled = false;
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void splash_timer_callback(void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Splash timer fired!");
  // show the main window
  window_stack_push(s_main_window, true);
  // remove the splash window from the stack
  window_stack_remove(s_splash_window, false);
  window_destroy(s_splash_window);
}

static void splash_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Splash layer contains the bitmap & text for the splash screen
  s_splash_layer = layer_create(bounds);
  layer_add_child(window_layer, s_splash_layer);

  // Create a text layer for the splash screen title
  s_heading_layer = text_layer_create(GRect(5, 15, bounds.size.w - 10, bounds.size.h));
  text_layer_set_font(s_heading_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_heading_layer, GColorWhite);
  text_layer_set_text_alignment(s_heading_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_heading_layer, GColorDarkCandyAppleRed);
  text_layer_set_text(s_heading_layer, "Alang");
  layer_add_child(s_splash_layer, text_layer_get_layer(s_heading_layer));
  
  // Create a bitmap layer for the splash screen logo
  s_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOGO);
  s_logo_layer = bitmap_layer_create(GRect(5, 15, bounds.size.w - 10, bounds.size.h));
  bitmap_layer_set_bitmap(s_logo_layer, s_logo_bitmap);
  layer_add_child(s_splash_layer, bitmap_layer_get_layer(s_logo_layer));
  
  // Start a timer to make the splash screen disappear
  s_splash_timer = app_timer_register(1500, splash_timer_callback, NULL);
}

static void splash_window_unload(Window *window) {
  // cleanup everything we created for the splash screen
  bitmap_layer_destroy(s_logo_layer);
  gbitmap_destroy(s_logo_bitmap);
  text_layer_destroy(s_heading_layer);
  layer_destroy(s_splash_layer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_prompt_layer = text_layer_create(GRect(5, 60, bounds.size.w - 10, bounds.size.h));
  text_layer_set_text(s_prompt_layer, "Please push the middle button to record");
  text_layer_set_font(s_prompt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_prompt_layer, GColorWhite);
  text_layer_set_text_alignment(s_prompt_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_prompt_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_prompt_layer));
}
 
static void main_window_unload(Window *window) {
}

static void init() {
  app_message_open(512, 512);
  s_splash_window = window_create();
  window_set_background_color(s_splash_window, GColorDarkCandyAppleRed);
  window_set_window_handlers(s_splash_window, (WindowHandlers) {
    .load = splash_window_load,
    .unload = splash_window_unload,
  });
  window_stack_push(s_splash_window, true);
  
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // TODO this doesn't need to happen until main_window_load, maybe move it?
  // Create new dictation session
  s_dictation_session = dictation_session_create(sizeof(s_last_text), 
                                                 dictation_session_callback, NULL);
  s_speaking_enabled = true;
}

static void deinit() {
  // Free the last session data
  dictation_session_destroy(s_dictation_session);

  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}