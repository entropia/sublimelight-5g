#! /usr/bin/env bash

set -e

if [[ -z ${MQTT_SERVER+x} ]];
then
   MQTT_SERVER="localhost"
fi

if [[ -z ${ID+x} ]];
then
    ID="99"
fi

commands=("BRIGHTNESS_AUTO -m 0"
"BRIGHTNESS_AUTO -m 500"
"BRIGHTNESS_AUTO -m 0"
"TEMPERATURE -m 0"
"BRIGHTNESS -m 500"
"BRIGHTNESS -m 0"
"TEMPERATURE -m 1"
"BRIGHTNESS -m 500"
"TEMPERATURE -m 0"
"BRIGHTNESS -m 0"
"WARM -m 500"
"WARM -m 0"
"COLD -m 500"
"COLD -m 0")

for c in "${commands[@]}";
do
    echo $c
    mosquitto_pub -r -h "${MQTT_SERVER}" -t cmnd/sl5g/id/${ID}/${c}
    sleep 1
done
