# Quiet Bathroom Fan

## Goal

My goal was to create a bathroom fan that would be very quiet and would be easy to clean. Removing the dust helps to keep the fan quiet.
The fan needed to be speed controlled, so the humidity could be removed quickly after a shower, and the rest of the time the air would not be renewed too fast (to avoid losing heat during winter, to be the quietest possible the rest of the time).

## Description

### Fan

This project started when I found out how quiet a good PC fan can be. So it was obvious for me to use a PC fan for that project. I chose 120mm fan, that size fits perfectly the hole in my bathroom wall.
A PC fan was also a perfect choice because most of the can be controlled with a PWM signal.

One of the concerns could be the humidity and the fan. I don’t think it will be a problem in the long term, since the goal of this project is to remove humidity. If the fan fails too quickly, I will change it for a model that can support more humidity (IP52 or even IP 67).

I went with a Noctua one as it has a good reputation, but other brands would work fine too. It is not clear which model is the best. Of course, the noise and the airflow speed are important parameters, but it seems that the static pressure should be important too (in case the wind tries to blow air in the opposite direction).
The NF-A12x25 PWM or NF-F12 PWM seem to be a good compromises. After testing the NF-F12 PWM, I finally got the NF-f12 PPC-3000 PWM. It looks like the exact same design than NF-F12 PWM, but it can run twice faster.

One important part is that kind of PC fan uses 12V.

### Case

I went for the simplest way to get something that fits exactly the need: 3D printing. I used OpenSCAD software. I know how to use it and that is definitely a good solution for that kind of project.

### Microcontroller

There are a lot of possible choices. I prefered to have a wifi microcontroller, to get temperature and humidity monitoring. That’s definitely not a requirement. Any small arduino like should work fine.
The extra feature was to be able to upload a new firmware without a USB cable. I knew I would adjust a lot the software at the beginning. I wanted to avoid having a USB cable hanging, or worse, having to open and close the fan box too often.
I think the popular ESP32 could be a good choice. But, in the past, I used few Particle Photon, so I used it again for that project.

With most of choices, 5V is needed to power the microcontroller (even if it works in 3.3V for input/output).

### Voltage

The big issue of this project is the voltage. This project has to be connected to 230V. Be aware, this can be very dangerous, and a mistake can be deadly.
Since the fan needs 12V, I chose to get from 230V to 12V, and then convert the 12V into 5V for the microcontroller.
The microcontroller inputs and outputs work in 3.3V. So I used the 3.3V from the microcontroller to power the pull-up resistors and the sensors (like the DHT22).

### Sensors

Of course a humidity sensor is needed. Since it is always easier to add stuff during design, it is better to add extra ones even if it is not useful now. The sensors:
- DHT22
- PIR sensor
- light sensor

The PIR is used to turn off LEDs when nobody is the bathroom. The light sensor is not used so far. All 3 sensors can work in 3.3V.

### LEDs

I knew I would spend time to adjust my software according to the humidity and the fan noise. To avoid having to use my laptop/phone to get values about the fan speed and humidity, I chose to add 8 NeoPixels from Adafruit. The goal is to display those values and have a quick look, so I can understand what is going on.
And also because it looks cool :).

### Components

- 240V to 12V
https://www.amazon.fr/gp/product/B017R4INRI/
- 12V to 5V
https://www.amazon.fr/dp/B07B7KCT44/
- PC Fan
https://www.amazon.fr/gp/product/B00KESS6O0/
- Temperature and humidity sensor
https://www.amazon.fr/Homyl-Dhumidit%C3%A9-Temp%C3%A9rature-Num%C3%A9rique-Adaptateur/dp/B07C9DRHL5/
- Photon particle
https://www.amazon.fr/Particle-PHOTONNOH-Photon-without-Headers/dp/B01BKOVIY6
- PIR sensor
https://www.amazon.fr/COM-four-pyroelektrischer-infrarouge-détecteur-mouvement/dp/B00W7CHFWU
- LDR
https://www.amazon.fr/qualité-Lumière-résistance-dépendant-photorésistance/dp/B00NXW9WZ6
- NeoPixel
https://www.amazon.fr/NeoPixel-Stick-WS2812-Integrated-Drivers/dp/B00KLBTT1E
- Fan filter
https://www.amazon.fr/gp/product/B00DL4BMVS

## 3D Model

![](3D/all.png)

To be able to print correctly the fan case, the model has been divided into 3 pieces: 
- the [pipe](3D/pipe.stl) for the exhaust fan
- the [lid](3D/lid.stl)
- the [case](3D/case.stl) itself
- the [LED windows](3D/LED.stl)

I chose to print the case in 2 colors, white filament, for the main part, and transparent filament for the 8 LED little windows. Those windows are not required. The case is big, but it can be reduced a little. I was able to print it on Ultimaker 3 extended.

The tube and the case have been glued with hot glue so it can be detached later if needed.
I wanted to screw the lid to the case, but it is not useful. I also wanted to use small magnets to keep the filter, but it is not needed either.
Both fit perfectly.

## Schema

![](schema/schema.png)

Fan
- PWM connected to D0
- Rotation pin connected to A3 with a pull-up 10k resistor to 3.3V

LDR
- 3.3V
- Pulldown 10k to ground and pin A0

NeoPixel (8 RGB pixels)
- pin: D1

PIR
- pin: D4
DHT 22 (humidity & temperature)
- pin: D5
- Pullup 10k to 3.3V

## Software

The software can be easily updated to have any configuration. So far, I configured my fan to run with the following settings:
- from  0% to  50% of humidity, fan speed is set at 20%
- from 50% to  65% of humidity, fan speed is set at 40%
- from 65% to 100% of humidity, fan speed is set at 75%

When the PIR sensor detects something, the LEDs are turned on. First the LEDs shows, in green color, the temperature, during 5 seconds. Between 20 and 28 Celsius, LEDs are turned on from bottom to top, with one LED per degres. Between 12 and 20 Celsius, LEDs are turned off from bottom to top with one LED per degres.

By default, the LEDs always shows, in red color, the humidity (range from 10% to 100%). If the PIR sensor detects nothing during 5mn the LEDs are turned off.

While the LEDs are on, if the fan speed changes, the LEDs shows, in white color, the fan speed (range from 10% to 100%), during 0.5s.
