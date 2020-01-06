#include <stdio.h>

#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>

#include "nvs_config.h"
#include "web_interface.h"
#include "wlan.h"

static const char *TAG = "SL5G_MAIN";

void app_main(void)
{
	ESP_LOGI(TAG, "Welcome to SublimeLight-5G");

	nvs_config_init();
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	web_interface_init();
	wlan_connect();
}
