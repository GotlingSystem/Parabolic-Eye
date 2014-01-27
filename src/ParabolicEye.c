/*
Parabolic Eye
 A Pebble watchface

Gotling System AB 2013

Hour is displayd in the middle as a circle with a triangle missing in the
 direction of the current hour

Minute and second are displayed by lines forming a parabolic curve. When the
 curve is full means 30 minutes/seconds.
 
Minute is in the top right corner
 
Second is in the bottom left corner
 
 */

#include "pebble.h"

Window *window;

Layer *display_layer;

#define X_MAX 144
#define Y_MAX 168
#define X_CEN 72
#define Y_CEN 84
#define BACKGROUND GColorBlack
#define FOREGROUND GColorWhite

static const GPathInfo HOUR_HAND_POINTS = {
	3, (GPoint []){
		{-10, -50},
		{10, -50},
		{0, 0}
	}
};
static GPath *hour_arrow;

void display_layer_update_callback(Layer *me, GContext* ctx) {
	
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	short x;
	short y;
	
	GPoint p0;
	GPoint p1;
	
	// Second
	// Top left, bottom left, bottom right
	if (t->tm_sec <= 30) {
		for (int i = 1;i <= t->tm_sec; i++) {
			x = i / 30.0 * X_MAX;
			y = i / 30.0 * Y_MAX;
			
			p0 = GPoint(0, y);
			p1 = GPoint(x, Y_MAX);
			
			graphics_context_set_stroke_color(ctx, FOREGROUND);
			graphics_draw_line(ctx, p0, p1);
		}
	} else {
		for (int i = 60;i >= t->tm_sec; i--) {
			x = (i-30) / 30.0 * X_MAX;
			y = (i-30) / 30.0 * Y_MAX;
			
			p0 = GPoint(0, y);
			p1 = GPoint(x, Y_MAX);
			
			graphics_context_set_stroke_color(ctx, FOREGROUND);
			graphics_draw_line(ctx, p0, p1);
		}
	}
	
	// Minute
	// Top left, top right, bottom right
	if (t->tm_min <= 30) {
		for (int i = 1; i <= t->tm_min; i++) {
			x = i / 30.0 * X_MAX;
			y = i / 30.0 * Y_MAX;
			
			p0 = GPoint(x, 0);
			p1 = GPoint(X_MAX, y);
			
			graphics_context_set_stroke_color(ctx, FOREGROUND);
			graphics_draw_line(ctx, p0, p1);
		}
	} else {
		for (int i = 60; i >= t->tm_min; i--) {
			x = (i-30) / 30.0 * X_MAX;
			y = (i-30) / 30.0 * Y_MAX;
			
			p0 = GPoint(x, 0);
			p1 = GPoint(X_MAX, y);
			
			graphics_context_set_stroke_color(ctx, FOREGROUND);
			graphics_draw_line(ctx, p0, p1);
		}
	}
	
	// Hour
	// Middle
	GPoint center = (GPoint) { .x = X_CEN, .y = Y_CEN};
	graphics_context_set_fill_color(ctx, FOREGROUND);
	graphics_fill_circle(ctx, center, 30);
	
	unsigned int angle = (t->tm_hour * 30);// + (t->tm_min / 2);
	gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, BACKGROUND);
	gpath_draw_filled(ctx, hour_arrow);
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(display_layer);
}


static void do_init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	window_set_background_color(window, BACKGROUND);
	
	Layer *root_layer = window_get_root_layer(window);
	GRect frame = layer_get_frame(root_layer);
	
	// Init the layer for the display
	display_layer = layer_create(frame);
	layer_set_update_proc(display_layer, &display_layer_update_callback);
	layer_add_child(root_layer, display_layer);
	
	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
	
	// Hour triangle
	hour_arrow = gpath_create(&HOUR_HAND_POINTS);
	gpath_move_to(hour_arrow, (GPoint) { .x = X_CEN, .y = Y_CEN});
}

static void do_deinit(void) {
	layer_destroy(display_layer);
	window_destroy(window);
	
	gpath_destroy(hour_arrow);
}


int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}
