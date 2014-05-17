macroScopeServer
================

http server for controlling Adafruit 32x32 led array
macroScopeServer - control an Adafruit LED array over http
Copyright 2014 - Zack Phillips - Duke University
zp15@duke.edu / zkphil@gmail.com

This project is licensed under the MIT License as described in the LICENSE file.

This project uses portions of the rpi-led-matrix-panel library (https://github.com/mattdh666/rpi-led-matrix-panel), used under the MIT License
This project includes the libmongoose http server library (https://code.google.com/p/mongoose/), used under the GNU GPL V2 License

Specific Requirements:
Raspberry Pi, model B, Rev 2 (Note early Rev 2 boards have a different GPIO Layout - most new ones should be fine)
Linux or Mac-based host PC (also possible to use Windows via PuTTY or similar, but I have not tried it)
Prototyping board, 26 pin (2x13 with 0.1" pitch) female header for GPIO, 16 pin (2x8 with 0.1" pitch male header
32x32 Adafruit LED Array (http://www.adafruit.com/products/607)
2x8 pin IDC ribbon cables (one per LED Array)
5V, 2A power supply

I  Adapter construction:
	See wiring diagrams at:
		https://github.com/mattdh666/rpi-led-matrix-panel
		http://elinux.org/images/2/2a/GPIOs.png
		https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/wiring-the-32x32-matrix

II. Raspberry Pi Setup:
	1. Install Raspbian on the pi (other distros should work as well) -- see http://www.raspbian.org/FrontPage
	2. Assign static IP to the raspberry pi (see http://www.suntimebox.com/raspberry-pi-tutorial-course/week-3/day-5/)
	3. Connect the pi's ethernet to your computer's ethernet port and log onto the pi.
	   To do this, open a terminal and run ssh -X pi@xxx.xxx.xxx.xxx where xxx.xxx.xxx.xxx is the ip address you assigned in the previous step. The default username is "pi" and the password is "raspberry"
	4. Copy macroScopeServer folder somewhere on the raspberry pi - either use a SD card reader to copy directly to the card, or use the command "scp -r (source dir) pi@xxx.xxx.xxx.xxx:(destination dir) on your linux host PC
	5. Change to the installed directory and run "make clean; make; sudo make install". This will build the program. You can then execute it using "./macroScopeServer"
	6. To automatically run the server as soon as the pi is booted up, log into the pi and copy the binary to the system path using "sudo cp /path/to/macroScopeServer /usr/local/bin", then run "leafpad /etc/rc.local". Add the line "sudo macroScopeServer" above the "exit 0" line.

III. Host PC Setup:
	1. Install curl: "sudo apt-get install curl"
        2. Set up ethernet network to use a static ip, but not the same ip as the raspberry pi.

IV. Usage:
	This program sets up a HTTP server that can be called using the command "curl" or by incorporating libcurl into existing programs. If you completed step 6 above, you sh
	 - From a terminal, run "curl --data "xPos=1&yPos=1&rVal=1&gVal=1&bVal=1" http://xxx.xxx.xxx.xxx:8000/singleLEDOn" where xxx.xxx.xxx.xxx is the ip of the Raspberry Pi.
	   This command should illuminate a single, white LED at the corner of the board. You can change xPos and yPos to change the position and use rVal, gVal, and bVal to change the color. By default, rVal/gVal/bVal can only be binary (no pwm)
         - Run "curl http://xxx.xxx.xxx.xxx:8000/clearAllLEDs" to clear the board at any point.
         - Run "curl --data "rVal=1&bVal=1&gVal=1" http://xxx.xxx.xxx.xxx:8000/fillScreen" to fill the screen with the specified color
         - Run "curl --data "xPos=1&yPos=2&rVal=1&gVal=1&gVal=1&text=test_text" http://xxx.xxx.xxx.xxx:8000/writeText" To write text "test_text" in the specified color at the position givne by xPos and yPos
         - To display an image, run "curl --data "image=25101111011111" http://xxx.xxx.xxx.xxx:8000/showImage
           Each pixel is passed as a continuous string of integers organized into blocks, of the format:
             XXYYRGB
             XX: the x position (uint)
             YY: the y position (uint)
             R: whether to illuminate the red LED (bool)
             G: whether to illuminate the green LED (bool)
             B: whether to illuminate the blue LED (bool)
             You can pass as many pixels as you need by concatonating many of these blocks together
         - The intended usage is from the terminal, but you can incorporate these into any language using a system call (e.g. system(COMMAND) in c++), or through something like libcurl.

V. Notes:
	- This program includes a precompiled version of the rpi-led-matrix-panel library, configured to NOT support PWM to reduce flicker and for support of 4 boards.
	- To change these configurations, obtain a copy of the library from (https://github.com/mattdh666/rpi-led-matrix-panel) and replace ./lib/librgbmatrix.a with the new version.