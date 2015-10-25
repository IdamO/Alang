#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_heading_layer, *s_prompt_layer;

static DictationSession *s_dictation_session;
static char s_last_text[256];

static bool s_speaking_enabled;
/*------------------------------------------------*/
// CString + key:
static const uint32_t dictionaryKey = 0xabbababe;
/*------------------------------------------------*/

static void next_question_handler(void *context) {
    s_speaking_enabled = true;
}

static void check_answer() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  // Write the CString:
  dict_write_cstring(iter, dictionaryKey, s_last_text);
  // End:
  // const uint32_t final_size = dict_write_end(iter);
  
    // Send the data!
  app_message_outbox_send();
  app_timer_register(3000, next_question_handler, NULL);
  vibes_double_pulse();
}

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, 
                                       char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    // Check this answer
    strncpy(s_last_text, transcription, sizeof(s_last_text));
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

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_heading_layer = text_layer_create(GRect(5, 5, bounds.size.w - 10, bounds.size.h));
  text_layer_set_font(s_heading_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_heading_layer, GColorWhite);
  text_layer_set_text_alignment(s_heading_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_heading_layer, GColorDarkCandyAppleRed);
  layer_add_child(window_layer, text_layer_get_layer(s_heading_layer));

  s_prompt_layer = text_layer_create(GRect(5, 80, bounds.size.w - 10, bounds.size.h));
  text_layer_set_text(s_heading_layer, "Welcome to Alang!");
  text_layer_set_text(s_prompt_layer, "Please push the middle button to record");
  text_layer_set_font(s_prompt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_prompt_layer, GColorWhite);
  text_layer_set_text_alignment(s_prompt_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_prompt_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_prompt_layer));
}

static void window_unload(Window *window) {
  
}

static void init() {
  app_message_open(512, 512);
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  // Create new dictation session
  s_dictation_session = dictation_session_create(sizeof(s_last_text), 
                                                 dictation_session_callback, NULL);
 
  window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
  s_speaking_enabled = true;
  
  /*------------------------------------------------*/
  // Calculate the buffer size that is needed for the final Dictionary:
// int data =1;
// const uint8_t key_count = 2;
// const uint32_t size = dict_calc_buffer_size(key_count, sizeof(data),
//                                             strlen(s_last_text) + 1);

// Stack-allocated buffer in which to create the Dictionary:
// uint8_t buffer[size];
  // Iterator variable, keeps the state of the creation serialization process:
// DictionaryIterator iter;
  /*------------------------------------------------*/
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