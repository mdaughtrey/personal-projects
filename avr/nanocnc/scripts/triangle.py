#!/usr/bin/python



import pdb
import Logo

def main():
#    pdb.set_trace()
    logo = Logo.Logo()
    logo.turnRight(45)
    print logo.walk(50)
    logo.turnRight(45)
    print logo.walk(50)
    print logo.home()

main()

