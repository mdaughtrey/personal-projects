---
title: 'Conway&#8217;s Game of Life on a LED Matrix'
date: '2010-04-06T23:13:26-04:00'
author: mdaughtrey
layout: post
enclosure:
    - "assets/uploads/2010/04/IMG_0304.mov\r\n1414306\r\nvideo/quicktime\r\n"
    - "assets/uploads/2010/04/IMG_0304.mov\r\n1414306\r\nvideo/quicktime"
categories:
    - 'Game of Life'
---

Following on from a previous post, here’s Conway’s [Game Of Life](http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) rendered on a 10×7 LED Matrix (the same on as I used [here](http://daughtrey.com/proj/heyes/HalloweenEyes.html)). I haven’t decided if I’m going to do the whole GOLidascope yet.

| [![](/assets/uploads/2010/04/gol-225x300.jpg "gol")](/assets/uploads/2010/04/gol.jpg) | [![](/assets/uploads/2010/04/p_2048_1536_F367D901-CFAF-4C37-89F2-5239CFDDCF21.jpeg "gol")](/assets/uploads/2010/04/p_2048_1536_F367D901-CFAF-4C37-89F2-5239CFDDCF21.jpeg) |
|---|---|

[Download a video](/assets/uploads/2010/04/IMG_0304.mov)

The Atmel ATMega169 on the back is the sole component; it’s handing the algorithm and the display multiplexing and running off its own internal 8MHz oscillator. Since there are no dedicated high current drivers and the chip can only handle so much current, a maximum of 2 LEDs per display are illuminated at any one time. The display is flicker free but there is a small variation in the brightness of the LEDs under some circumstances.

The displays are ancient (1998) bicolor Kingbright matrices I got off Ebay several years ago. I have a few hundred in the basement.

There are just enough pins left to enable the module to communicate with 4 neighbors so it’ll build up quite nicely if I do a bunch of them.
