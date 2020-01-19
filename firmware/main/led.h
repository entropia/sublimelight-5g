#ifndef SL5G_LED_H
#define SL5G_LED_H

typedef enum {
	WARM_WHITE,
	COLD_WHITE
} led_color_t;

static const int CHANNELS = 2;

void led_init(void);

void led_set(led_color_t color, unsigned intensity);

void led_fallback(void);

#endif
