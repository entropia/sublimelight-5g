#include "web_interface.h"

#include <esp_event.h>
#include <esp_http_server.h>
#include <esp_https_ota.h>
#include <esp_log.h>

#include <freertos/timers.h>

#include "git_revision.h"
#include "nvs_config.h"

static const char *TAG = "SL5G_WEB_INTERFACE";

static httpd_handle_t server = NULL;

extern const char info_page_html[];
extern size_t info_page_html_length;

static esp_err_t request_too_large(httpd_req_t *req)
{
	httpd_resp_set_status(req, "413 Payload Too Large");
	httpd_resp_sendstr(req, "413 Payload Too Large");

	return ESP_OK;
}

static esp_err_t bad_request(httpd_req_t *req)
{
	httpd_resp_set_status(req, "400 Bad Request");
	httpd_resp_sendstr(req, "400 Bad Request");

	return ESP_OK;
}

static esp_err_t url_decode_in_place(char *string)
{
	char *read = string;
	char *write = string;

	while (*read) {
		if (*read == '%') {
			if (!read[1] || !read[2]) {
				// Input ended prematurely after '%'
				return ESP_ERR_INVALID_ARG;
			}
			char code[3] = { read[1], read[2], '\0' };
			unsigned int result;
			if (sscanf(code, "%x", &result) != 1) {
				return ESP_ERR_INVALID_ARG;
			}
			*write = (char) result;
			write++;
			read += 3;
		} else if (*read == '+') {
			*write = ' ';
			write++;
			read++;
		} else {
			*write = *read;
			write++;
			read++;
		}
	}

	*write = '\0';

	return ESP_OK;
}

static esp_err_t handle_get_root(httpd_req_t *req)
{
	nvs_config_t *config = nvs_config_get();

	char *html = NULL;
	int written = asprintf(&html, info_page_html,
			       GIT_REVISION_ID, __DATE__ " " __TIME__,
	                       config->device_id, config->room_name, config->mqtt_broker_uri);
	assert(written > 0);

	httpd_resp_send(req, html, written);
	free(html);

	return ESP_OK;
}

static esp_err_t handle_post_updateconfig(httpd_req_t *req)
{
	char *req_data = malloc(req->content_len + 1);
	if (!req_data) {
		return request_too_large(req);
	}

	httpd_req_recv(req, req_data, req->content_len);
	req_data[req->content_len] = '\0';

	ESP_LOGI(TAG, "/updateconfig POST: %s", req_data);

	char *req_value = malloc(req->content_len + 1);
	if (!req_data) {
		free(req_data);
		return request_too_large(req);
	}

	nvs_config_t *config = nvs_config_get();

	esp_err_t ret = httpd_query_key_value(req_data, "boardid", req_value, req->content_len);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	ret = url_decode_in_place(req_value);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	config->device_id = strdup(req_value);

	ret = httpd_query_key_value(req_data, "roomname", req_value, req->content_len);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	ret = url_decode_in_place(req_value);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	config->room_name = strdup(req_value);

	ret = httpd_query_key_value(req_data, "broker", req_value, req->content_len);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	ret = url_decode_in_place(req_value);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	config->mqtt_broker_uri = strdup(req_value);

	nvs_config_update(config);

	httpd_resp_set_status(req, "303 See Other");
	httpd_resp_set_hdr(req, "Location", "/");
	httpd_resp_sendstr(req, "Configuration updated");

	free(req_data);
	free(req_value);
	nvs_config_free(config);

	return ESP_OK;

out_bad_request:
	free(req_data);
	free(req_value);
	nvs_config_free(config);
	return bad_request(req);
}

static void reboot_timer_tick(TimerHandle_t timer)
{
	esp_restart();
}

static esp_err_t handle_post_updatefirmware(httpd_req_t *req)
{
	char *req_data = malloc(req->content_len + 1);
	if (!req_data) {
		return request_too_large(req);
	}

	httpd_req_recv(req, req_data, req->content_len);
	req_data[req->content_len] = '\0';

	ESP_LOGI(TAG, "/updatefirmware POST: %s", req_data);

	char *req_value = malloc(req->content_len + 1);
	if (!req_data) {
		free(req_data);
		return request_too_large(req);
	}

	esp_err_t ret = httpd_query_key_value(req_data, "imageuri", req_value, req->content_len);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}
	ret = url_decode_in_place(req_value);
	if (ret != ESP_OK) {
		goto out_bad_request;
	}

	ESP_LOGI(TAG, "Updating firmware from %s", req_value);

	esp_http_client_config_t ota_client = {
		.url = req_value,
	};
	ret = esp_https_ota(&ota_client);
	if (ret == ESP_OK) {
		httpd_resp_sendstr(req, "Firmware update OK, Rebooting...<br><a href=\"/\">go back</a>");

		// Give the HTTP server some time to send the answer
		// before we turn off the lights.
		TimerHandle_t delay_reboot_timer = xTimerCreate("delay_reboot_timer", pdMS_TO_TICKS(1000), pdFALSE, NULL, reboot_timer_tick);
		xTimerStart(delay_reboot_timer, 10);
	} else {
		httpd_resp_set_status(req, "500 Internal Server Error");
		httpd_resp_sendstr(req, "Firmware update failed");
	}

	free(req_data);
	free(req_value);
	return ESP_OK;

out_bad_request:
	free(req_data);
	free(req_value);
	return bad_request(req);
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
		.method = HTTP_POST,
		.handler = handle_post_updateconfig,
		.user_ctx = NULL,
	};
	ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri));

	uri = (httpd_uri_t) {
		.uri = "/updatefirmware",
		.method = HTTP_POST,
		.handler = handle_post_updatefirmware,
		.user_ctx = NULL,
	};
	ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri));

	return server;
}

static void stop_web_interface(httpd_handle_t *server)
{
	// BUG double-free is possible here.
	httpd_stop(*server);
	// TODO check whether this helps
	*server = NULL;
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
		stop_web_interface(server);
	}
}

esp_err_t web_interface_init()
{
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_dhcp_got_ip, &server));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, &server));

	return ESP_OK;
}
