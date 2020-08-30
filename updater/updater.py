#!/usr/bin/env python3

import argparse
import collections
import dataclasses
import enum
import functools
import http
import http.client
import http.server
import ipaddress
import queue
import re
import shutil
import socket
import sys
import threading
import urllib

import paho.mqtt.client as mqtt

HTTP_LISTEN_HOST = ""
HTTP_PORT = 8000
HTTP_LOCAL_PATH = "/firmware.bin"

SL5G_IP_TOPIC = "stat/sl5g/id/+/IP"
SL5G_PANEL_REGEX = "stat/sl5g/id/([0-9]+)/IP"
SL5G_UPDATE_ENDPOINT = "/updatefirmware"

def get_my_ip(test_host):
    """Returns the IPv4 address this host uses when connecting to
    test_host. If that address is a loopback address tries again using
    1.1.1.1 as test_host."""

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect((test_host, 1))
        result = s.getsockname()[0]

    if not ipaddress.ip_address(result).is_loopback:
        return result

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect(("1.1.1.1", 1))
        return s.getsockname()[0]


def start_panel_update(panel, event_queue):
    my_ip = get_my_ip(panel.ip)

    panel_url = f"http://{panel.ip}{SL5G_UPDATE_ENDPOINT}"
    firmware_download_url = f"http://{my_ip}:{HTTP_PORT}{HTTP_LOCAL_PATH}"

    def runner():
        form_data = urllib.parse.urlencode({"imageuri": firmware_download_url}).encode("ascii")
        req = urllib.request.Request(panel_url, data=form_data)
        event_queue.post_update_requested(panel.panel_id)
        with urllib.request.urlopen(req):
            pass

    update_thread = threading.Thread(target=runner, name=f"Panel {panel.panel_id} update request thread")
    update_thread.start()


class EventQueue():
    FoundIP = collections.namedtuple("FoundIp", "panel_id ip")
    UpdateRequested = collections.namedtuple("UpdateRequested", "panel_id")
    DownloadStarted = collections.namedtuple("DownloadStarted", "ip")
    Shutdown = collections.namedtuple("Shutdown", "")

    Panel = dataclasses.make_dataclass("Panel", ["panel_id", "ip", "state"])
    PanelState = enum.Enum("IPState", "NEW UPDATE_REQUESTED DOWNLOAD_STARTED REBOOTED")


    def __init__(self):
        self.queue = queue.Queue(maxsize=1)
        self.panels = []


    def start(self):
        def runner():
            while True:
                msg = self.queue.get()
                if type(msg) == self.Shutdown:
                    return
                elif type(msg) == self.FoundIP:
                    self.handle_found_ip(msg)
                elif type(msg) == self.UpdateRequested:
                    self.handle_update_requested(msg)
                elif type(msg) == self.DownloadStarted:
                    self.handle_download_started(msg)

                self.print_panels()

        queue_thread = threading.Thread(target=runner, name="Event queue thread")
        queue_thread.start()


    def print_panels(self):
        print("| id | IP address      | state            |")
        print("+----+-----------------+------------------+")
        for p in self.panels:
            print("| {:<2} | {:<15} | {:<16} |".format(p.panel_id, p.ip, p.state.name))
        print("")

    def handle_found_ip(self, msg):
        panel_id = msg.panel_id
        match = [p for p in self.panels if p.panel_id == msg.panel_id]

        if match != []:
            # We have seen this panel before, assume it has rebooted and is therefore done.
            # However, remember that it may have got another IP address this time.
            panel = match[0]
            if panel.state in [self.PanelState.DOWNLOAD_STARTED, self.PanelState.REBOOTED]:
                panel.ip = msg.ip
                panel.state = self.PanelState.REBOOTED
            else:
                print("!!! Panel {}: IP changed ({} -> {}) before reboot was due".format(panel_id, panel.ip, msg.ip))
                states[panel_id] = prev_state._update(ip=msg.ip)

        else:
            # This is a new panel. Make a note of it and request it to update
            new_panel = self.Panel(panel_id, msg.ip, self.PanelState.NEW)
            self.panels.append(new_panel)
            start_panel_update(new_panel, self)


    def handle_update_requested(self, msg):
        panel_id = msg.panel_id
        match = [p for p in self.panels if p.panel_id == msg.panel_id]

        if match == []:
            print("!!! Panel {}: Not yet known but update request already sent".format(panel_id))

        else:
            panel = match[0]
            if panel.state == self.PanelState.NEW:
                panel.state = self.PanelState.UPDATE_REQUESTED
            elif panel.state == self.PanelState.DOWNLOAD_STARTED:
                # This is not a problem, the DownloadStarted message has just overtaken us
                pass
            else:
                print("!!! Panel {}: Spurious request sent".format(panel_id))


    def handle_download_started(self, msg):
        ip = msg.ip
        match = [p for p in self.panels if p.ip == msg.ip]

        if match == []:
            print("!!! Unknown download from {}".format(ip))

        else:
            panel = match[0]
            if panel.state in [self.PanelState.NEW, self.PanelState.UPDATE_REQUESTED]:
                # If we see NEW, we have overtaken the UpdateRequested message.
                # This is not a problem.
                panel.state = self.PanelState.DOWNLOAD_STARTED
            else:
                print("!!! Panel {}: Spurious download".format(panel.id))


    def post_found_ip(self, panel_id, ip):
        self.queue.put(self.FoundIP(panel_id, ip))


    def post_update_requested(self, panel_id):
        self.queue.put(self.UpdateRequested(panel_id))


    def post_download_started(self, ip):
        self.queue.put(self.DownloadStarted(ip))


    def post_shutdown(self):
        self.queue.put(self.Shutdown())


def start_event_queue():
    queue = EventQueue()
    queue.start()

    return queue


def start_binary_server(event_queue, binary_path):
    """Launches an HTTP server for the binary."""

    class HTTPHandler(http.server.BaseHTTPRequestHandler):
        def log_message(self, *args):
            pass

        def do_GET(self):

            if self.path != HTTP_LOCAL_PATH:
                self.send_response(http.HTTPStatus.NOT_FOUND, "Not found")
                self.end_headers()
                self.wfile.write("Not found\n".encode("utf-8"))
                return

            binary_file = open(binary_path, "rb")

            event_queue.post_download_started(self.client_address[0])

            self.send_response(http.HTTPStatus.OK)
            self.end_headers()

            shutil.copyfileobj(binary_file, self.wfile)


    httpd = http.server.ThreadingHTTPServer((HTTP_LISTEN_HOST, HTTP_PORT), HTTPHandler)

    def serve():
        httpd.serve_forever()
        print("HTTP server has shut down")

    httpd_thread = threading.Thread(target=serve, name="HTTP server thread")
    httpd_thread.start()

    return httpd


def start_mqtt_listener(event_queue, mqtt_server_host, mqtt_port):

    def on_mqtt_connect(client, userdata, flags, rc):
        client.subscribe(SL5G_IP_TOPIC)

    def on_mqtt_publish(client, userdata, msg):
        panel_id = re.match(SL5G_PANEL_REGEX, msg.topic).group(1)
        event_queue.post_found_ip(panel_id, msg.payload.decode("utf-8"))


    client = mqtt.Client()
    client.on_connect = on_mqtt_connect
    client.on_message = on_mqtt_publish

    client.connect(mqtt_server_host, mqtt_port, 60)
    client.loop_start()

    return client


def main():
    parser = argparse.ArgumentParser(description="Updater for sublimelight-5")
    parser.add_argument("binary", metavar="BINARY",
                        help="Path to the new firmware binary")
    parser.add_argument("-m", "--mqtt-server", metavar="MQTT_SERVER",
                        dest="mqtt_server", action="store", default="localhost",
                        help="Hostname/IP address of the MQTT server where the light modules publish their status")
    parser.add_argument("-p", "--mqtt-port", metavar="MQTT_PORT",
                        dest="mqtt_port", type=int, action="store", default=1883,
                        help="Listening port of the MQTT server where the light modules publish their status")
    args = parser.parse_args()

    event_queue = start_event_queue()
    binary_server = start_binary_server(event_queue, args.binary)
    mqtt_listener = start_mqtt_listener(event_queue, args.mqtt_server, args.mqtt_port)

    input()

    event_queue.post_shutdown()
    binary_server.shutdown()
    mqtt_listener.loop_stop()

    return 0


if __name__ == "__main__":
    sys.exit(main())
