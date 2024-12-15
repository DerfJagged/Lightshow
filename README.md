# Lightshow
Lightshow is an app that allows you to test the individual LEDs on your Xbox 360's Ring of Light (ROL) as well as the center power LED.

Credit to landaire for [smc.h](https://github.com/landaire/LaunchCode/blob/master/LaunchCode/smc.h) and [smc.cpp](https://github.com/landaire/LaunchCode/blob/master/LaunchCode/smc.cpp). Credit to Xenomega for their [plugin/symlink code](https://github.com/Xenomega/ArchLoader/tree/master).

<img src="https://consolemods.org/wiki/images/5/58/Lightshow_Picture.png" width="49%" height="auto">

## Usage

Hold the Left Stick in the direction of the LED you'd like to modify and press a button to change the color to green, yellow, red, or off.

Press Start to toggle the center LED on or off.

If all ROL LEDs are set to off and a controller is connected, the system will turn Player 1's LED to green. This is a feature of the system, and cannot be overridden by the app.

![](./Lightshow_Demo.gif)

## Creating Lightshows

As of Version 2, you have the option to create lightshows and even set them to run at bootup by setting the included `lightshow-boot.xex` as a plugin in DashLaunch. Note that the included lightshow.conf must be located in `hdd1:\Apps\Lightshow\` for the plugin to find it. To test a lightshow, you can press Start in the main app and it will play any lightshow.conf found in the directory containing the app.

If you have a midi file with 4 or less instruments, you can name it `song.mid`, place it in the same directory as `lightshow_maker.py` on your PC, and run the python script to generate a lightshow.conf. It will take approximately double the length of the song to process, but will automatically color code notes for each instruments based on the observed peaks and lows. Note that long songs will eventually go out of sync as it takes a non-zero amount of time each time the LED is changed. Because of this, the video below had to have segments sped up at different speeds to compensate — though this isn't an issue if you're just making short boot animations.

Click either image below to see demos of the app in action:

[![PS2 Boot Remix demo](https://img.youtube.com/vi/-H91NT_gmmU/0.jpg)](https://www.youtube.com/watch?v=-H91NT_gmmU "Xbox 360 Lightshow Demo :: PS2 Boot Remix")
[![Sandstorm demo](https://img.youtube.com/vi/3WJwQjNUcpw/0.jpg)](https://www.youtube.com/watch?v=3WJwQjNUcpw "Xbox 360 Lightshow Demo :: Darude - Sandstorm")
