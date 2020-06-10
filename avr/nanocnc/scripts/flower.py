#!/usr/bin/python



import pdb
import Logo

def main():
#    pdb.set_trace()
    logo = Logo.Logo()
    logo.turnRight(45)
    logo.walk(50)
    logo.push()

    for xx in range(0,4):
        logo.walk(20)
        logo.turnRight(90)
        logo.walk(20)
        logo.walkToPop()
        logo.push()
#        logo.turnRight(90)

main()

