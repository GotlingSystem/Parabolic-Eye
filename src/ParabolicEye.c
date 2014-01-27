/*
 
Draw lines to produce parabolic curves
 
 */

#include "pebble.h"

Window *window;

Layer *display_layer;

#define X_MAX 144
#define Y_MAX 168
#define X_CEN 72
#define Y_CEN 84

char buf[3];

unsigned short get_display_hour(unsigned short hour) {
	if (clock_is_24h_style()) {
		return hour / 2;
	} else {
		return hour;
	}
}

void display_layer_update_callback(Layer *me, GContext* ctx) {
	
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	short sec_x;
	short sec_y;
	
	GPoint p0;
	GPoint p1;
	
	// Second
	// Top left, bottom left, bottom right
	if (t->tm_sec <= 30) {
		for (int i = 1;i <= t->tm_sec; i++) {
			sec_x = i / 30.0 * X_MAX;
			sec_y = i / 30.0 * Y_MAX;
			
			p0 = GPoint(0, sec_y);
			p1 = GPoint(sec_x, Y_MAX);
			
			graphics_context_set_stroke_color(ctx, GColorWhite);
			graphics_draw_line(ctx, p0, p1);
		}
	} else {
		for (int i = 60;i >= t->tm_sec; i--) {
			sec_x = (i-30) / 30.0 * X_MAX;
			sec_y = (i-30) / 30.0 * Y_MAX;
			
			p0 = GPoint(0, sec_y);
			p1 = GPoint(sec_x, Y_MAX);
			
			graphics_context_set_stroke_color(ctx, GColorWhite);
			graphics_draw_line(ctx, p0, p1);
		}
	}
	
	short min_x;
	short min_y;
	
	// Minute
	// Top left, top right, bottom right
	if (t->tm_min <= 30) {
		for (int i = 1; i <= t->tm_min; i++) {
			min_x = i / 30.0 * X_MAX;
			min_y = i / 30.0 * Y_MAX;
			
			p0 = GPoint(min_x, 0);
			p1 = GPoint(X_MAX, min_y);
			
			graphics_context_set_stroke_color(ctx, GColorWhite);
			graphics_draw_line(ctx, p0, p1);
		}
	} else {
		for (int i = 60; i >= t->tm_min; i--) {
			min_x = (i-30) / 30.0 * X_MAX;
			min_y = (i-30) / 30.0 * Y_MAX;
			
			p0 = GPoint(min_x, 0);
			p1 = GPoint(X_MAX, min_y);
			
			graphics_context_set_stroke_color(ctx, GColorWhite);
			graphics_draw_line(ctx, p0, p1);
		}
	}
	
	// Hour
	// Middle
	snprintf(buf, 3, "%d", t->tm_hour);
	
	GRect frame = GRect(X_CEN-20, Y_CEN-10, 40, 20);
	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, buf,
					   fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
					   frame,
					   GTextOverflowModeTrailingEllipsis,
					   GTextAlignmentCenter,
					   NULL);
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(display_layer);
}


static void do_init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	window_set_background_color(window, GColorBlack);
	
	Layer *root_layer = window_get_root_layer(window);
	GRect frame = layer_get_frame(root_layer);
	
	// Init the layer for the display
	display_layer = layer_create(frame);
	layer_set_update_proc(display_layer, &display_layer_update_callback);
	layer_add_child(root_layer, display_layer);
	
	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void do_deinit(void) {
	layer_destroy(display_layer);
	window_destroy(window);
}


int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}
