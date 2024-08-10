# Vibration Mole Repellent device

## Why use a vibration mole repellent?
Vibration mole repellent devices are nothing new, they are on the market for over 30 years. They are also very simple, a vibration motor and some basic logic that turns them on.
Gardens with big dogs in them rarely have moles. The dogs create vibrations in the ground as they run and moles naturally avoid these gardens.
This device aims to mimic this effect.

## So why build your own?
Three reasons:
- You can tweak the vibration pattern so it will be like a dog running. As far as I know commercial ones don't have this feature or if they have its just a lame sequence.
- You can put very strong motors in it to affect bigger area - if you want.
- Infinetely repairable

> This device is going to be more expensive than a store bought mole repellent! It is only going to be somewhat economically viable if you can source 18650 batteries and make yourself a 3s battery pack.

![Image of finished product](https://github.com/dansity/vibration_mole_repellent/blob/main/img/IMG_20240617_173346206.jpg)

## Tools you need for the project
- Soldering iron
- PC with Arduino IDE v2 installed
- Glue gun
- Screw driver
- 3d printer optional
- Spot welder for batteries optional

## Shoppig list
(Aliexpress links)
- 12V vibration motor (the one with weight on both ends): https://www.aliexpress.com/item/1005005000354424.html
- 12V MPPT charging module: https://www.aliexpress.com/item/1005005997380493.html
- Solar Panel: https://www.aliexpress.com/item/1005007031699718.html
- Mosfet swith: https://www.aliexpress.com/item/1005006457613501.html
- Waterproof rocker switch: https://www.aliexpress.com/item/1005005695423032.html
- Cheapest ESP you can find ESP32-C3: https://www.aliexpress.com/item/1005006406538478.html
- Voltage sensor module: https://www.aliexpress.com/item/1005006385487339.html
- 3S BMS module: https://www.aliexpress.com/item/1005003986215534.html
- 3.3V step down module: https://www.aliexpress.com/item/1005007136976280.html

## Source locally
- 32mm PVC pipe, about 200mm length needed
- 0.14mm wires, black, red and one more color
- 75x75mm waterproof electronics box from electronics supply store

## Software
1. In Arduinio IDE v2 download Espressif ESP32 boards library (the one by Espressif not the Arduino)
2. Check and tweak settings if needed in the beginning of the code
3. Set USB CDC On BOOT - Disabled (else deep sleep pin state will not preserved). You can lower CPU clock if you want for extra power saving.
4. Flash firmware

## Hardware
Use attached wiring schematic
![Wiring schematic of mole repellent](https://github.com/dansity/vibration_mole_repellent/blob/main/img/ksnip_20240619-144350.png)

Use Pin 0 for the voltage sensor and Pin 3 for the mosfet gate pin

Use a 3.3V step down module, don't trust the linear voltage regulators on cheap microcontrollers! They don't last long.

I rolled everything in kapton tape so they dont short and shoved it into the box.
Use irresponsible amount of hot glue to make everything stick together (optional)
Check images of my device in img directory.
