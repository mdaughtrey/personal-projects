#!/bin/env python3

# https://nitratine.net/blog/post/encryption-and-decryption-in-python/#generating-a-key-from-a-password
import base64
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.fernet import Fernet
import json
import sys

jsonfile='config.json.new'

def genkey(provided):
    password = provided.encode()
    salt = b'adeff'
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend())

    key = base64.urlsafe_b64encode(kdf.derive(password))
    return key

def enc():
    password = input('Encryption password: ')
    key = genkey(password)
    f = Fernet(key)
    text = input('Enter plaintext: ')
    print(f.encrypt(text.encode()))

def dec():
    password = input('Decryption password: ')
    key = genkey(password)
    f = Fernet(key)
    text = input('Enter encrypted text: ')
    print(f.decrypt(text.encode()))

def main():
    if 'dec' == sys.argv[1]: dec()
    elif 'enc' == sys.argv[1]: enc()
    else: print('What?')
    sys.exit(1)

main()
    

