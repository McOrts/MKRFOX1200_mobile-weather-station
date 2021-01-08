# Mobile IOT weather station based on Arduino MKRFOX1200
The arduino MKRFOX1200 with connection to Sigfox has taken the democratization of the use of microcontrollers to the space of the IOT and the cloud.

Based on this microprocessor **I build a mobile weather station for UV, pressure and temperature measures in anywhere**. This compact device can collect environment information each 15 minutes autonomously for three months with three AA batteries and almost anywhere in the world. 

![MKRFOX1200_mobile-weather-station](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/MKRFOX1200_mobile-weather-station.jpg?raw=true)
The device is finally **placed on the beach of Palma de Mallorca in Spain**. So you can see right now the index of ultraviolet radiation, temperature and atmospheric pressure in the following link: https://thingspeak.com/channels/365024

This project will put on your cell phone the basic weather information collected in real time by a mobile device that you can "abandon" in a large part of the world.

## IOT Concept

The Internet of Things is a network of connected devices which communicate over the Internet, and they do so autonomously, machine to machine, without the need for human intervention.

> The first reference to the IoT was in 1982, when researchers at Carnegie Mellon University developed the world’s first IoT-enabled Coke Machine. Mark Weiser developed the concept further in the early 90s; and Kevin Ashton coined the term ‘Internet of Things’ around 1999.

All IOT architecture is built with 5 components. I will list these components explaining the implementation I have made to build this device.

### Sensors
The world can be watched with a large array of sensors to choose from. Today they are invisible and energy efficient, whilst maintaining a high measurement precision. I used these:
* [ML8511](https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide) Ultra violet sensor
* [BMP180](https://www.adafruit.com/product/1603) Temperature and pressure sensors

![ML8511](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/ML8511_UV_Sensor.jpg?raw=true)
![BMP180](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/BMP180_TemPre_Sensor.jpg?raw=true)

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

![https://www.sigfox.com/en/coverage](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/sigfox_Global_Coverage.png)

On my Christmas holidays I had the opportunity to try out the device in the different cities that I have visited: Madrid, Salamanca, Warsaw and Krakow. Also It was a few days above the level of 1000m in the Sierra de Tramuntana (Mallorca). All the information collected can be found at: (https://github.com/McOrts/MKRFOX1200_mobile-weather-station/tree/master/MKRFOX1200_mobile-weather-station_report-data)

![Example of my device location](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/MKRFOX1200_mobile-weather-station_report-data/MKRFOX1200_location_Poland_Krakow_201801.png)

### Platform
Third, the collected data needs to be stored and processed somewhere. Known as IoT platforms, these are typically cloud-based infrastructures which:

1. receive and send data via standardised interfaces, known as API;
2. store the data; and
3. process the data.

For this project I chose [ThingSpeak](https://thingspeak.com). platform. A free account allows you to store up to 8 fields as long as the limit of 3,000 bytes is not exceeded with a maximum of 3 million messages per year.

### Analytics
Some data analytics needs to be applied to the data as the value is not in the raw bits and bytes, but rather in the insights gathered from them.

![Mallorca data](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/MKRFOX1200_mobile-weather-station_report-data/MKRFOX1200_graph_Spain_Mallorca_Palma_201711.png)

[ThingSpeak Visualizations](https://thingspeak.com/channels/365024) can not only show graphs of your data. Also can do an analysis of your data using MATLAB programming. For example, calculate the dew point based on temperature and humidity. 

```
% Humidity and temperature are read from a ThingSpeak channel to calculate
% dew point.

% Channel ID to read data from
readChannelID = 365024;
% Humidity Field ID
HumidityFieldID = 2;
% Temperature Field ID
TemperatureFieldID = 3;

% Channel Read API Key 
readAPIKey = 'UTVU9A464TKW6UQ5';

% TODO - Replace the [] with channel ID to write data to:
writeChannelID = 365024;
% TODO - Enter the Write API Key between the '' below:
writeAPIKey = '****************';

tempF = thingSpeakRead(readChannelID, 'Fields', TemperatureFieldID, 'ReadKey', readAPIKey);

% Get latest humidity data from the MathWorks Weather Station channel
humidity = thingSpeakRead(readChannelID, 'Fields', HumidityFieldID, 'ReadKey', readAPIKey);

% Convert temperature from Fahrenheit to Celsius
tempC = (5/9)*(tempF-32);

% Calculate dew point
% Specify the constants for water vapor (b) and barometric (c) pressure.
b = 17.62;
c = 243.5;
% Calculate the intermediate value 'gamma'
gamma = log(humidity/100) + b*tempC ./ (c+tempC);
% Calculate dew point in Celsius
dewPoint = c*gamma ./ (b-gamma);
% Convert to dew point in Fahrenheit
dewPointF = (dewPoint*1.8) + 32;
display(dewPointF, 'Dew point')

% Write the dew point value to another channel specified by the
% 'writeChannelID' variable

thingSpeakWrite(writeChannelID, dewPointF, 'Writekey', writeAPIKey);

```
In addition, ThingSpeak allows you to  act on the data by triggering events such as twitter messages.

### User Interface
An important component is how the data is presented to the final users. There are appealing user interfaces, both web based as well as smart phone or tablet based. 

I have chosen an application that offers a widget in iOS:

![Widget app with UV measure](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/cell_widget.png?raw=true)

## Schematic and Connections
I assembled all the components for the prototype over a [MKR Proto Large Shield](https://store.arduino.cc/mkr-proto-large-shield). This is the breadboard:

![Breadboard](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/MKRFOX1200_mobile-weather-station_Breadboard.png)

## Architecture
This device collects environmental data and send the information as a discrete radio packets each 15 minutes. The radio network ingests the data in internet network through rest-api calls. So finally, websites and mobile applications can process and display de information. 

![Architecture](https://github.com/McOrts/MKRFOX1200_mobile-weather-station/blob/master/MKRFOX1200_mobile-weather-station_Architecture.png)

## Acknowledgements
To Luis del Valle of https://programarfacil.com for the blog entry:
(https://programarfacil.com/podcast/sigfox-arduino-mkrfox1200-radiacion-uv/)
