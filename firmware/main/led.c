#include "led.h"

#include <driver/ledc.h>
#include <esp_log.h>

#undef USE_ONBOARD_LEDS

static const char *TAG = "SL5G_LED";

ledc_channel_config_t channels[2] = {
	[WARM_WHITE] = {
		.channel = LEDC_CHANNEL_0,
		.duty = 0,
#ifdef USE_ONBOARD_LEDS
		.gpio_num = 22,
#else
		.gpio_num = 32,
#endif
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.hpoint = 0,
		.timer_sel = LEDC_TIMER_0,
	},
	[COLD_WHITE] = {
		.channel = LEDC_CHANNEL_1,
		.duty = 0,
#ifdef USE_ONBOARD_LEDS
		.gpio_num = 23,
#else
		.gpio_num = 33,
#endif
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.hpoint = 0,
		.timer_sel = LEDC_TIMER_0,
	},
};

void led_init()
{
	ledc_timer_config_t ledc_timer = {
		.duty_resolution = LEDC_TIMER_13_BIT,
		.freq_hz = CONFIG_SL5G_PWM_FREQ,
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.timer_num = LEDC_TIMER_0,
		.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config(&ledc_timer);

	for (int i = 0; i < CHANNELS; i++) {
		ledc_channel_config(&channels[i]);
	}
}

void led_set(led_color_t color, unsigned intensity)
{
	ESP_LOGI(TAG, "Setting channel %i to %i", color, intensity);
	ledc_set_duty(channels[color].speed_mode, channels[color].channel, intensity);
	ledc_update_duty(channels[color].speed_mode, channels[color].channel);
}

void led_fallback(void)
{
	ESP_LOGI(TAG, "Setting LEDs to fallback levels");

	ledc_set_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel,
		      CONFIG_SL5G_FALLBACK_WARM_INTENSITY);
	ledc_update_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel);

	ledc_set_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel,
		      CONFIG_SL5G_FALLBACK_COLD_INTENSITY);
	ledc_update_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel);
}
