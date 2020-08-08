# USB keyboard
The code implements a composite USB mouse and keyboard device using USB HID class and FRDM KL25Z.

The device registers keystrokes from a touchscreen and uses the onboard accelrometer to register motion for mouse support.
 
This USB code is based on <a href="http://kevincuzner.com/2014/12/12/teensy-3-1-bare-metal-writing-a-usb-driver/#freescale-usb">Kevin Cuzner's</a> article.
