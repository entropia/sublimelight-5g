#!/usr/bin/env python3

import argparse
import collections
import curses
import dataclasses
import enum
import http
import http.client
import http.server
import ipaddress
import queue
import re
import shutil
import socket
import time
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
    test_host. If that address is a loopback address, tries again using
    1.1.1.1 as test_host."""

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.connect((test_host, 1))
        result = sock.getsockname()[0]

    if not ipaddress.ip_address(result).is_loopback:
        return result

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.connect(("1.1.1.1", 1))
        return sock.getsockname()[0]


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
    RegisterShutdownHook = collections.namedtuple("RegisterShutdownHook", "hook")
    Shutdown = collections.namedtuple("Shutdown", "")

    Panel = dataclasses.make_dataclass("Panel", ["panel_id", "ip", "state"])
    PanelState = enum.Enum("IPState", "NEW UPDATE_REQUESTED DOWNLOADING REBOOTED")

    def __init__(self, curses_window):
        curses_window.nodelay(True)

        self.curses_window = curses_window
        self.queue = queue.Queue()
        self.panels = []
        self.shutdown_hooks = []
        self.messages = []


    def run(self):
        try:
            while True:
                in_char = self.curses_window.getch()
                if in_char == 10:
                    break

                try:
                    msg = self.queue.get(block=False)
                except queue.Empty:
                    continue

                if isinstance(msg, self.Shutdown):
                    break
                elif isinstance(msg, self.FoundIP):
                    self.handle_found_ip(msg)
                elif isinstance(msg, self.UpdateRequested):
                    self.handle_update_requested(msg)
                elif isinstance(msg, self.DownloadStarted):
                    self.handle_download_started(msg)
                elif isinstance(msg, self.RegisterShutdownHook):
                    self.handle_register_shutdown_hook(msg)

                self.print_panels()
        finally:
            for hook in self.shutdown_hooks:
                hook()


    def print_panels(self):
        result = ""
        result += "| id | IP address      | state            |\n"
        result += "+----+-----------------+------------------+\n"
        for p in self.panels:
            result += "| {:<2} | {:<15} | {:<16} |\n".format(p.panel_id, p.ip, p.state.name)

        result += "\nPress Enter to exit. Warning: Do not exit when panels are DOWNLOADING\n\n"

        for m in self.messages:
            result += m + "\n"

        self.curses_window.clear()
        self.curses_window.addstr(0, 0, result)
        self.curses_window.refresh()


    def handle_found_ip(self, msg):
        panel_id = msg.panel_id
        match = [p for p in self.panels if p.panel_id == msg.panel_id]

        if match != []:
            # We have seen this panel before, assume it has rebooted and is therefore done.
            # However, remember that it may have got another IP address this time.
            panel = match[0]
            panel.ip = msg.ip
            if panel.state in [self.PanelState.DOWNLOADING, self.PanelState.REBOOTED]:
                panel.state = self.PanelState.REBOOTED
            else:
                self.messages.append("!!! Panel {}: IP changed ({} -> {}) before reboot was due".format(panel_id, panel.ip, msg.ip))

        else:
            # This is a new panel. Make a note of it and request it to update
            new_panel = self.Panel(panel_id, msg.ip, self.PanelState.NEW)
            self.panels.append(new_panel)
            start_panel_update(new_panel, self)


    def handle_update_requested(self, msg):
        panel_id = msg.panel_id
        match = [p for p in self.panels if p.panel_id == msg.panel_id]

        if match == []:
            self.messages.append("!!! Panel {}: Not yet known but update request already sent".format(panel_id))

        else:
            panel = match[0]
            if panel.state == self.PanelState.NEW:
                panel.state = self.PanelState.UPDATE_REQUESTED
            elif panel.state == self.PanelState.DOWNLOADING:
                # This is not a problem, the DownloadStarted message has just overtaken us
                pass
            else:
                self.messages.append("!!! Panel {}: Spurious request sent".format(panel_id))


    def handle_download_started(self, msg):
        ip = msg.ip
        match = [p for p in self.panels if p.ip == msg.ip]

        if match == []:
            self.messages.append("!!! Unknown download from {}".format(ip))

        else:
            panel = match[0]
            if panel.state in [self.PanelState.NEW, self.PanelState.UPDATE_REQUESTED]:
                # If we see NEW, we have overtaken the UpdateRequested message.
                # This is not a problem.
                panel.state = self.PanelState.DOWNLOADING
            else:
                self.messages.append("!!! Panel {}: Spurious download".format(panel.panel_id))


    def handle_register_shutdown_hook(self, msg):
        self.shutdown_hooks.append(msg.hook)


    def post_found_ip(self, panel_id, ip):
        self.queue.put(self.FoundIP(panel_id, ip))


    def post_update_requested(self, panel_id):
        self.queue.put(self.UpdateRequested(panel_id))


    def post_download_started(self, ip):
        self.queue.put(self.DownloadStarted(ip))


    def post_register_shutdown_hook(self, hook):
        self.queue.put(self.RegisterShutdownHook(hook))


    def post_shutdown(self):
        self.queue.put(self.Shutdown())


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

    try:
        httpd = http.server.ThreadingHTTPServer((HTTP_LISTEN_HOST, HTTP_PORT), HTTPHandler)
    except OSError as e:
        print("Could not open HTTP server: {}".format(e))
        event_queue.post_shutdown()

    def serve():
        httpd.serve_forever()

    def shutdown():
        httpd.shutdown()

    event_queue.post_register_shutdown_hook(shutdown)

    httpd_thread = threading.Thread(target=serve, name="HTTP server thread")
    httpd_thread.start()

    return httpd


def start_mqtt_listener(event_queue, mqtt_server_host, mqtt_port):

    def on_mqtt_connect(client, _userdata, _flags, _rc):
        client.subscribe(SL5G_IP_TOPIC)

    def on_mqtt_publish(_client, _userdata, msg):
        panel_id = re.match(SL5G_PANEL_REGEX, msg.topic).group(1)
        event_queue.post_found_ip(panel_id, msg.payload.decode("utf-8"))


    client = mqtt.Client()
    client.on_connect = on_mqtt_connect
    client.on_message = on_mqtt_publish

    try:
        client.connect(mqtt_server_host, mqtt_port, 60)
    except OSError as e:
        print("Could not connect to MQTT server: {}".format(e))
        event_queue.post_shutdown()

    def shutdown():
        client.loop_stop()

    event_queue.post_register_shutdown_hook(shutdown)

    client.loop_start()

    return client


def main(stdscr):
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

    stdscr.nodelay(True)

    event_queue = EventQueue(stdscr)
    binary_server = start_binary_server(event_queue, args.binary)
    mqtt_listener = start_mqtt_listener(event_queue, args.mqtt_server, args.mqtt_port)

    event_queue.run()


if __name__ == "__main__":
    curses.wrapper(main)
