#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"
#include <stdlib.h>
#include "resource_ids.auto.h"

#define MY_UUID HTTP_UUID

PBL_APP_INFO(MY_UUID, "External IP", "Daniel Carll", 1, 0,  RESOURCE_ID_IMAGE_MENU_ICON_BLACK, APP_INFO_STANDARD_APP);

int32_t cookie=00000;

char *strrev(char *);
char *itoa(int, char *, int);
int32_t random_number();
int32_t get_cookie(int32_t);

void handle_init(AppContextRef ctx);
void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context);
void http_failure(int32_t request_id, int http_status, void* context);
void window_appear(Window* me);
void httpebble_error(int error_code);

Window window;
TextLayer layer_text1;
TextLayer layer_text2;
TextLayer layer_text3;
TextLayer layer_text4;
TextLayer layer_text5;

void pbl_main(void *params) {
    get_cookie(cookie);
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .messaging_info = {
            .buffer_sizes = {
                .inbound = 124,
                .outbound = 256,
            }
        }
    };

    app_event_loop(params, &handlers);
}

void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context) {
    if (request_id != cookie) {
        return;
    }
    text_layer_set_text(&layer_text1, "IP detected:");
    Tuple* tuple1 = dict_find(received, 0);
    Tuple* tuple2 = dict_find(received, 1);
    text_layer_set_text(&layer_text2, tuple1->value->cstring);

    static char* printed="1111111222222223333332222";
    strcpy(printed,"Device seen: ");
    strcat(printed, tuple2->value->cstring);
    strcat(printed, " times.");
    static char* cookie2="999999";
	snprintf(cookie2, strlen(cookie2), "%ld", cookie);
    static char* newcookie="HTTP COOKIE:      ";
    strcpy(newcookie,"HTTP COOKIE: ");
    strcat(newcookie,cookie2);
    text_layer_set_text(&layer_text3, printed);
    text_layer_set_text(&layer_text4, newcookie);
	text_layer_set_text(&layer_text5, "You may now close this app. It will wait for httpebble requests in the background.");
}

void http_failure(int32_t request_id, int http_status, void* context) {
    httpebble_error(http_status >= 1000 ? http_status - 1000 : http_status);
}

void window_appear(Window* me) {
    static char* url="http://ip.tocloud.us/pebip.php?cookie=123123";
	static char* cookie2="999999";
	snprintf(cookie2, strlen(cookie2), "%ld", cookie);
	strcpy(url,"http://ip.tocloud.us/pebip.php?cookie=");
	strcat(url,cookie2);
	DictionaryIterator* dict;
    HTTPResult  result = http_out_get(url, cookie, &dict);
    if (result != HTTP_OK) {
        httpebble_error(result);
        return;
    }

    result = http_out_send();
    if (result != HTTP_OK) {
        httpebble_error(result);
        return;
    }
}

void handle_init(AppContextRef ctx) {
    http_set_app_id(76782702);
    resource_init_current_app(&APP_RESOURCES);

    http_register_callbacks((HTTPCallbacks) {
        .success = http_success,
         .failure = http_failure
    }, NULL);

    window_init(&window, "External IP");
    window_stack_push(&window, true);
    window_set_window_handlers(&window, (WindowHandlers) {
        .appear  = window_appear
    });

    text_layer_init(&layer_text1, GRect(0, 2, 144, 30));
    text_layer_set_text_color(&layer_text1, GColorBlack);
    text_layer_set_background_color(&layer_text1, GColorClear);
    text_layer_set_font(&layer_text1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(&layer_text1, GTextAlignmentCenter);
    layer_add_child(&window.layer, &layer_text1.layer);

    text_layer_init(&layer_text2, GRect(0, 28, 144, 30));
    text_layer_set_text_color(&layer_text2, GColorBlack);
    text_layer_set_background_color(&layer_text2, GColorClear);
    text_layer_set_font(&layer_text2, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(&layer_text2, GTextAlignmentCenter);
    layer_add_child(&window.layer, &layer_text2.layer);

    text_layer_init(&layer_text3, GRect(0, 50, 144, 30));
    text_layer_set_text_color(&layer_text3, GColorBlack);
    text_layer_set_background_color(&layer_text3, GColorClear);
    text_layer_set_font(&layer_text3, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(&layer_text3, GTextAlignmentCenter);
    layer_add_child(&window.layer, &layer_text3.layer);

    text_layer_init(&layer_text4, GRect(0, 122, 144, 30));
    text_layer_set_text_color(&layer_text4, GColorBlack);
    text_layer_set_background_color(&layer_text4, GColorClear);
    text_layer_set_font(&layer_text4, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(&layer_text4, GTextAlignmentCenter);
    layer_add_child(&window.layer, &layer_text4.layer);

    text_layer_init(&layer_text5, GRect(0, 70, 144, 70));
    text_layer_set_text_color(&layer_text5, GColorBlack);
    text_layer_set_background_color(&layer_text5, GColorClear);
    text_layer_set_font(&layer_text5, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(&layer_text5, GTextAlignmentLeft);
    text_layer_set_overflow_mode(&layer_text5, GTextOverflowModeWordWrap);
    layer_add_child(&window.layer, &layer_text5.layer);

}

void httpebble_error(int error_code) {

    static char error_message[] = "UNKNOWN_HTTP_ERRROR_CODE_GENERATED";

    switch (error_code) {
    case HTTP_SEND_TIMEOUT:
        strcpy(error_message, "HTTP_SEND_TIMEOUT");
        break;
    case HTTP_SEND_REJECTED:
        strcpy(error_message, "HTTP_SEND_REJECTED");
        break;
    case HTTP_NOT_CONNECTED:
        strcpy(error_message, "HTTP_NOT_CONNECTED");
        break;
    case HTTP_BRIDGE_NOT_RUNNING:
        strcpy(error_message, "HTTP_BRIDGE_NOT_RUNNING");
        break;
    case HTTP_INVALID_ARGS:
        strcpy(error_message, "HTTP_INVALID_ARGS");
        break;
    case HTTP_BUSY:
        strcpy(error_message, "HTTP_BUSY");
        break;
    case HTTP_BUFFER_OVERFLOW:
        strcpy(error_message, "HTTP_BUFFER_OVERFLOW");
        break;
    case HTTP_ALREADY_RELEASED:
        strcpy(error_message, "HTTP_ALREADY_RELEASED");
        break;
    case HTTP_CALLBACK_ALREADY_REGISTERED:
        strcpy(error_message, "HTTP_CALLBACK_ALREADY_REGISTERED");
        break;
    case HTTP_CALLBACK_NOT_REGISTERED:
        strcpy(error_message, "HTTP_CALLBACK_NOT_REGISTERED");
        break;
    case HTTP_NOT_ENOUGH_STORAGE:
        strcpy(error_message, "HTTP_NOT_ENOUGH_STORAGE");
        break;
    case HTTP_INVALID_DICT_ARGS:
        strcpy(error_message, "HTTP_INVALID_DICT_ARGS");
        break;
    case HTTP_INTERNAL_INCONSISTENCY:
        strcpy(error_message, "HTTP_INTERNAL_INCONSISTENCY");
        break;
    case HTTP_INVALID_BRIDGE_RESPONSE:
        strcpy(error_message, "HTTP_INVALID_BRIDGE_RESPONSE");
        break;
    default: {
        strcpy(error_message, "HTTP_ERROR_UNKNOWN");
    }
    }
    text_layer_set_text(&layer_text1, error_message);
}

char *strrev(char *str) {
    char *p1, *p2;
    if (!str || !*str)
        return str;

    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}
char *itoa(int n, char *s, int b) {
    static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i=0, sign;
    if ((sign = n) < 0)
        n = -n;
    do {
        s[i++] = digits[n % b];
    } while ((n /= b) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    return strrev(s);
}

int32_t get_cookie(int32_t s) {
    s=random_number();
    cookie=s;
    return s;
}

int32_t random_number()
{
    int32_t result=0,low_num=0,hi_num=0;
    int32_t min_num=10000,max_num=99999;
    if(min_num<max_num)
    {
        low_num=min_num;
        hi_num=max_num+1; // this is done to include max_num in output.
    } else {
        low_num=max_num+1;// this is done to include max_num in output.
        hi_num=min_num;
    }
    srand(time(NULL));
    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}
