#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

static const int WARM_WHITE = 0;
static const int COLD_WHITE = 1;

static const int CHANNELS = 2;

void app_main(void)
{
	printf("Hello world\n");

	ledc_timer_config_t ledc_timer = {
		.duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
		.freq_hz = 5000,                      // frequency of PWM signal
		.speed_mode = LEDC_HIGH_SPEED_MODE,   // timer mode
		.timer_num = LEDC_TIMER_0,            // timer index
		.clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
	};

	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t channels[2] = {
		// warm white channel
		{
			.channel = LEDC_CHANNEL_0,
			.duty = 0,
			.gpio_num = 32,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = LEDC_TIMER_0,
		},
		// cold white channel
		{
			.channel = LEDC_CHANNEL_1,
			.duty = 0,
			.gpio_num = 33,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = LEDC_TIMER_0,
		},
	};

	for (int i = 0; i < CHANNELS; i++) {
		ledc_channel_config(&channels[i]);
	}

	while (1) {
		printf("Warm white ramp up...\n");
		for (int duty = 0; duty < 9000; duty += 500) {
			printf("%d\n", duty);
			ledc_set_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel, duty);
			ledc_update_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
		printf("Warm white off\n");
		ledc_set_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel, 0);
		ledc_update_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel);

		printf("Cold white ramp up...\n");
		for (int duty = 0; duty < 9000; duty += 500) {
			printf("%d\n", duty);
			ledc_set_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel, duty);
			ledc_update_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
		printf("Cold white off\n");
		ledc_set_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel, 0);
		ledc_update_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel);

		printf("Both ramp up...\n");
		for (int duty = 0; duty < 9000; duty += 500) {
			printf("%d\n", duty);
			ledc_set_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel, duty);
			ledc_set_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel, duty);
			ledc_update_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel);
			ledc_update_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
		printf("Both off\n");
		ledc_set_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel, 0);
		ledc_set_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel, 0);
		ledc_update_duty(channels[WARM_WHITE].speed_mode, channels[WARM_WHITE].channel);
		ledc_update_duty(channels[COLD_WHITE].speed_mode, channels[COLD_WHITE].channel);
	}
}
