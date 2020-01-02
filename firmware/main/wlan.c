#include "wlan.h"

#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>

static const char *TAG = "SL5G_WLAN";

static void on_wifi_event(void* arg, esp_event_base_t event_base,
			  int32_t event_id, void* event_data)
{
	(void)arg;
	(void)event_data;

	assert(event_base == WIFI_EVENT);

	if (event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
		ESP_LOGI(TAG, "Wifi connection initiated");
	} else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(TAG, "Wifi connection lost");
		esp_wifi_connect();
		ESP_LOGI(TAG, "Wifi connection re-initiated");
	}
}

esp_err_t wlan_connect(void)
{
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &on_wifi_event, NULL));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = CONFIG_SL5G_WIFI_SSID,
			.password = CONFIG_SL5G_WIFI_PASSWORD,
		},
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(TAG, "Wifi system started");
	return ESP_OK;
}
