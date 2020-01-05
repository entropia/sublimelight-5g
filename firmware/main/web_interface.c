#include "web_interface.h"

#include <esp_event.h>
#include <esp_http_server.h>
#include <esp_log.h>

#include "nvs_keys.h"

static const char *TAG = "SL5G_WEB_INTERFACE";

static httpd_handle_t server = NULL;

extern const char info_page_html[];
extern size_t info_page_html_length;

static esp_err_t handle_get_root(httpd_req_t *req)
{
	nvs_handle_t nvsh;
	esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvsh);

	httpd_resp_send(req, info_page_html, info_page_html_length);
	return ESP_OK;
}

static httpd_handle_t start_web_interface()
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	esp_err_t err = httpd_start(&server, &config);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Could not start server: %s", esp_err_to_name(err));
		return NULL;
	}

	httpd_uri_t uri;

	uri = (httpd_uri_t) {
		.uri = "/",
		.method = HTTP_GET,
		.handler = handle_get_root,
		.user_ctx = NULL,
	};
	ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri));

	uri = (httpd_uri_t) {
		.uri = "/updateconfig",
		.method = HTTPD_POST,
		.handler = handle_post_updateconfig,
		.user_ctx = NULL,
	};

	return server;
}

static void stop_web_interface(httpd_handle_t server)
{
	httpd_stop(server);
}

static void on_dhcp_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	httpd_handle_t *server = (httpd_handle_t*)arg;

	if (*server == NULL) {
		ESP_LOGI(TAG, "Starting HTTP server");
		*server = start_web_interface();
	} else {
		ESP_LOGW(TAG, "Got new IP but HTTP server already running");
	}
}

static void on_wifi_disconnect(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	httpd_handle_t *server = (httpd_handle_t*)arg;

	if (*server != NULL) {
		ESP_LOGI(TAG, "Stopping HTTP server after Wifi disconnect");
		stop_web_interface(*server);
	}
}

esp_err_t web_interface_init()
{
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_dhcp_got_ip, &server));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, &server));

	return ESP_OK;
}
