#!/usr/bin/python

from PIL import Image, ImageDraw

im = Image.open('input.jpg').convert('L')
(imX, imY) = im.size
im = im.point(lambda ii: ii > 190 and 255)

for ff in range(4,11):
		xfile = im.copy()
		id = ImageDraw.Draw(xfile)
		for gg in range(0, imX, imX/ff):
				id.line((gg, 0, gg, imY), fill=255)
		xfile.save('xfile_%u.jpg' % ff)

im.save('output.jpg')

