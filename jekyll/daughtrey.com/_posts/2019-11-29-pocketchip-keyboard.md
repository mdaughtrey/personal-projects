---
title: 'PocketChip Keyboard'
date: '2019-11-29T23:11:31-05:00'
author: mdaughtrey
layout: post
categories:
    - 'PocketChip Keyboard'
---

![Pocket Chip](/assets/uploads/2019/11/IMG_20191129_220911-768x1024.jpg)
The Pocket Chip is fun to play with but that keyboard is nearly unusable. I designed a magnetically attached removable replacement that goes on top of the metal domes and provides a good positive click action. I find 3D printed keys are pretty annoying and I needed space to print the key legends, so I’ve used genuine buttoncaps. They feel better and leave room for legends.

![OpenSCAD keyboard model](/assets/uploads/2019/11/openscad.png)
I found the PCB layout files and extracted the pad coordinates to design a keyboard. The hexagonal and round projections fit into the holes in the Pocket Chip.

![Keyboard top side](/assets/uploads/2019/11/IMG_20191107_204530-1024x768.jpg)
Here is the print. I sanded the top and then ironed it to provide a smooth surface for the vinyl to stick to.

![](/assets/uploads/2019/11/IMG_20191107_204547-1024x768.jpg)
Here’s the underside showing the projections, the holes for the keys and the parts for the magnets.

![](/assets/uploads/2019/11/IMG_20191107_221343-1024x768.jpg)
![](/assets/uploads/2019/11/IMG_20191107_224708-1024x768.jpg)
Here is the CriCut vinyl printer/cutter printing legends on the adhesive vinyle sheet. I modified the cutter to use permanent markers (the provided markers are water-based and won’t print on vinyl at all), but the ink will still smear after a while so I cut clear vinyl to provide a protective layer. I used the extracted pad coordinates as input into a script to programmatically generate an SVG file which can be imported into CriCut’s design software. This is pushing what the design software is built to do so there were a lot of iterations.

![](/assets/uploads/2019/11/IMG_20191107_225038-1024x768.jpg)
I made a template to position the vinyl layer and let me accurately place it on the keyboard.

![](http://daughtrey.com/wp-content/uploads/2019/11/IMG_20191110_142257-1024x768.jpg)
The keys took a lot of experimentation with different parts to get right. The top part is ***100PCS For 6\*6Round Tact Switch A101 Plastic Switch Button Caps*** from Ali Express, link [here](https://www.aliexpress.com/item/32989256441.html?spm=a2g0s.9042311.0.0.6f544c4dt0uqft). The inserts are ***Fastex 207-080551-00 Plastic Plug 1/8 Hole Round Head***. They fit together to make a solid key with a good click feel.

![](/asserts/uploads/2019/11/IMG_20191110_143829-1024x768.jpg)
Here’s the populated keyboard. You can also see the magnets used to hold the keyboard onto the pocket chip. At the bottom of the image are retaining bars which, once glued in place, stop the keys from falling out.

![Pocket Chip with Keyboard](/assets/uploads/2019/11/IMG_20191129_220943-768x1024.jpg)
Here’s the completed unit. I glued magnets onto the underside of the board, and between those and the round/hexagonal inserts.

This isn’t quite finished as the legends look pretty rough. The Cricut is really at its limit drawing characters this small, so I think it would be better to print onto laser print onto a large label and protect that with the clear vinyl layer. If this is the last paragraph then I haven’t done that yet.

The OpenSCAD files are at <https://github.com/mdaughtrey/openscad/tree/master/pocketchip>
