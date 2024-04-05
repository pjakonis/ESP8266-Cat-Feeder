# ESP8266 Cat Feeder

![Alt text for the GIF](https://github.com/pjakonis/ESP8266-Cat-Feeder/blob/main/feed.gif)

This project is an automated cat feeder powered by the ESP8266 microcontroller. It utilizes a web server hosted on the ESP8266 to set specific feeding times or to manually trigger the feeding mechanism.

## Features

- **Web Interface**: A user-friendly web interface to set feeding times and manually control the feeder.
- **Automatic Feeding Schedule**: Set up to two specific times for automatic feeding.
- **Manual Feeding Option**: A manual option to trigger the feeding mechanism at any time.
- **Time Synchronization**: Utilizes NTP to synchronize the current time, ensuring the feeding schedule is accurate.

## Hardware Requirements

- ESP8266 Microcontroller
- Servo Motor
- Button (optional for manual override)
- Power Supply for the ESP8266 and servo motor
- A mechanism for dispensing cat food, connected to the servo

## Usage

- **Set Feeding Times**: Use the web interface to set up to two feeding times. These times are when the servo will activate to dispense food.
- **Manual Feeding**: Click the "Open And Feed The Monster" button on the web interface to trigger the servo manually.
- **View and Delete Set Times**: The web interface displays the currently set feeding times, which can be deleted if needed.
