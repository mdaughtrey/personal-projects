#!/usr/bin/python

import Logo

def main():
    logo = Logo.Logo()
    logo.walk(20)
    logo.turnRight(90);
    for xx in range(0,4):
        logo.walk(49)
        logo.turnLeft(90)
        logo.walk(15)
        logo.turnLeft(180)
        logo.walk(15)
        logo.turnLeft(90)


main()

