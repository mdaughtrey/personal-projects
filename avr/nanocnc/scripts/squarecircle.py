#!/usr/bin/python



import pdb
import Logo

def main():
#    pdb.set_trace()
    logo = Logo.Logo()
    for xx in range(0,2):
        logo.walk(30)
        logo.turnRight(90)
        logo.walk(15)
        logo.turnLeft(90)
        logo.walk(30)
        logo.turnLeft(90)
        logo.walk(15)
        logo.turnRight(90)

    for yy in range(0,16):
        logo.turnRight(22.5)
        for xx in range(0,4):
            logo.walk(40)
            logo.turnRight(90)

main()

