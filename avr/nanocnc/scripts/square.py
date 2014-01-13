#!/usr/bin/python



import pdb
import Logo

def main():
#    pdb.set_trace()
    logo = Logo.Logo()
    for xx in range(0, 4):
        print logo.walk(10)
        logo.turnRight(90)


main()

