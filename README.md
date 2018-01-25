# Mobile weather station based on Arduino MKRFOX1200
The arduino MKRFOX1200 with connection to Sigfox has taken the democratization of the use of microcontrollers to the space of the IOT and the cloud.

![alt text](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/MKRFOX1200_mobile-weather-station.jpg?raw=true)

This project will put on your cell phone the basic weather information collected in real time by a mobile device that you can "abandon" in a large part of the world.

## IOT Concept

The Internet of Things is a network of connected devices which communicate over the Internet, and they do so autonomously, machine to machine, without the need for human intervention.

The first reference to the IoT was in 1982, when researchers at Carnegie Mellon University developed the world’s first IoT-enabled Coke Machine. Mark Weiser developed the concept further in the early 90s; and Kevin Ashton coined the term ‘Internet of Things’ around 1999.

All IOT architecture is built with 5 components. I will list these components explaining the implementation I have made to build this device.

### Sensors
The world can be watched with a large array of sensors to choose from. Today they are invisible and energy efficient, whilst maintaining a high measurement precision. I used these:
* [ML8511](https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide) Ultra violet sensor
* [BMP180](https://www.adafruit.com/product/1603) Temperature and pressure sensors

### Connectivity
Once we have the sensons, we need to connect it reliably to the Internet. But now, wireless connectivity is central to the success of the IoT
Here Sigfox opens the door to connectivity to inaccessible areas for other technologies such as 3 / 4G, GSM or Wifi.
It uses a free frequency band that in Europe is 868 MHz and in the United States it is 915 MHz. It is a narrow band connection that allows very low energy consumption.
The transmission limits are 140 messages per day of no more than 24 bytes. If we divide the 140 messages between 24 hours a day, we have to send a message every 10.2 minutes. This is an example of a sent frame:

```
Frame:			
  4b594e40|0e|00fa|314204740

Decoded values:
  3.2242|14|1018|3.111333
  Battery|Temperature|Presure|UV
```

Additionally, it offers a localization service  based on a proprietary probabilistic calculation of the most probable location of the device. This approach allows for the first release of the service to have an accuracy in km (between 1km and 10km for 80% of devices) depending on the base station density where the device is located. This accuracy will be improved through further releases thanks to machine learning.

This is the global coverage of the sigfox network:

![alt text](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/sigfox_Global_Coverage.png?raw=true)
https://www.sigfox.com/en/coverage

### Platform
Third, the collected data needs to be stored and processed somewhere. Known as IoT platforms, these are typically cloud-based infrastructures which:

1. receive and send data via standardised interfaces, known as API;
2. store the data; and
3. process the data.

For this project I chose [ThingSpeak](https://pages.github.com/). platform. A free account allows you to store up to 8 fields as long as the limit of 3,000 bytes is not exceeded with a maximum of 3 million messages per year.


![Widget app with UV measure](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/cell_widget.png?raw=true)
