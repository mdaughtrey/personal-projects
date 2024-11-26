---
title: 'Findings, partially solved'
date: '2012-04-12T23:08:01-04:00'
author: mdaughtrey
layout: post
categories:
    - Projects
    - timelapse
    - Time Lapse Video Throwie
---

I’ve modified the code so that the timing is now driven off of the internal counter/timer instead of using a delay loop. This means that the variable image processing time is at least partially resolved, at least while the image processing time  
is smaller than the image capture interval. For instance if I have a 15 second capture interval then while the image processing takes less than about 12 seconds, it’s fine. After that the image capture interval grows.

I’ve also modified the code to have the chip wait for a button press on the power switch before it starts working. This means that you can charge the unit and not have it capture images until you are ready. It also tests the power button in between image captures so you can turn it off anytime. I’m not going to bother putting the chip to sleep between intervals since it only draws about 0.5ma when busy.

I think that’s about it for the features I wanted to put in. The next step is to put the Atmel into the case and put the camera unit back together. I have some units coming from Hong Kong so hopefully they’ll arrive soon.
