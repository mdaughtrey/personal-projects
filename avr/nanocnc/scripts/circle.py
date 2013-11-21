#!/usr/bin/python



import pdb
import Logo

def main():
#    pdb.set_trace()
    logo = Logo.Logo()
    logo.turnRight(90)
    for xx in range(0,180):
        logo.walk(2)
        logo.turnRight(2)

main()

