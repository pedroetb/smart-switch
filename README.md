# smart-switch

Smart switch for AC, based on Arduino

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Sponsor](https://img.shields.io/badge/-Sponsor-fafbfc?logo=GitHub%20Sponsors)](https://github.com/sponsors/pedroetb)

## Description

TODO

## HTTP communication

TODO

## MQTT communication

TODO

### MQTT usage examples

Using default configuration for MQTT, you can interact with smart-switch like this:

```sh
#Listen output MQTT messages from specific smart-switch
mosquitto_sub -h "<broker>" -t "pedroetb/smart-switch/001/output"

#Listen log MQTT messages from specific smart-switch
mosquitto_sub -h "<broker>" -t "pedroetb/smart-switch/001/log"

#Listen output MQTT messages from any smart-switch
mosquitto_sub -h "<broker>" -v -t "pedroetb/smart-switch/+/output"

#Listen log MQTT messages from any smart-switch
mosquitto_sub -h "<broker>" -v -t "pedroetb/smart-switch/+/log"

#Listen all MQTT messages (input, output and log) from/to any smart-switch
mosquitto_sub -h "<broker>" -v -t "pedroetb/smart-switch/#"

#Send any action ("/status" for example) as MQTT message to specific smart-switch
mosquitto_pub -h "<broker>" -t "pedroetb/smart-switch/001/input" -m "/status"
```

## License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

This project is released under the [MIT License](LICENSE).

## References

* [BUILD a Smart Switch With an Arduino and a Relay - YouTube](https://www.youtube.com/watch?v=7swG4XVSx50)
* [NodeMCU, la popular placa de desarrollo con ESP8266](https://www.luisllamas.es/esp8266-nodemcu/)
* [Turn on and Off a Lamp Clapping Twice, Using Arduino : 4 Steps - Instructables](https://www.instructables.com/Turn-on-and-Off-a-Lamp-Clapping-Twice-Using-Arduin/)
* [Cómo emplear el ESP8266 o ESP32 como servidor HTTP](https://www.luisllamas.es/como-emplear-el-esp8266-como-servidor/)
* [Arduino Tutorial: Avoiding the Overflow Issue When Using millis() and micros()](https://www.norwegiancreations.com/2018/10/arduino-tutorial-avoiding-the-overflow-issue-when-using-millis-and-micros/)
* [ESP8266 Arduino: Software restart - techtutorialsx](https://techtutorialsx.com/2017/12/29/esp8266-arduino-software-restart/)
* [Instalar un interruptor, conmutador o cruzamiento - Bricolajero](https://bricolajero.com/instalar-un-interruptor-conmutador-o-cruzamiento/)
* [Detector de cruce por cero con Arduino y optoacoplador H11AA1](https://www.luisllamas.es/arduino-cruce-por-cero-h11aa1/)
* [pubsubclient/mqtt_large_message.ino at master · knolleary/pubsubclient](https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_large_message/mqtt_large_message.ino)
* [Cómo programar el ESP8266 o ESP32 por WiFi con Arduino OTA](https://www.luisllamas.es/como-programar-el-esp8266-por-wifi-con-arduino-ota/)
* [H11aa1 y Arduino : Midiento la frecuencia de red. - YouTube](https://www.youtube.com/watch?v=7xisoSWYbOA)
* [Control de Fase con Arduino : Diseño del Cirrcuito. - YouTube](https://www.youtube.com/watch?v=fX3HQqQkMic)
* [The Evils of Arduino Strings | Majenko's Hardware Hacking Blog](https://hackingmajenkoblog.wordpress.com/2016/02/04/the-evils-of-arduino-strings/)
* [Memory, memory, always memory !!! – An Esp8266 microcontroller in my Life](https://esp8266life.wordpress.com/2019/01/13/memory-memory-always-memory/)
* [Optimizing SRAM | Memories of an Arduino | Adafruit Learning System](https://learn.adafruit.com/memories-of-an-arduino/optimizing-sram)
* [Template Metaprogramming: Compile time loops over class methods | by Niko Savas | Medium](https://medium.com/@savas/template-metaprogramming-compile-time-loops-over-class-methods-a243dc346122)
