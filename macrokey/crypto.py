#!/bin/env python3

# https://nitratine.net/blog/post/encryption-and-decryption-in-python/#generating-a-key-from-a-password
import base64
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.fernet import Fernet
import pdb

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
    password = input('Enter password: ')
    key = genkey(password)
    f = Fernet(key)
    data = open('test.json', 'rb').read()
    open('test.json', 'wb').write(f.encrypt(data))

def dec():
    password = input('Enter password: ')
    key = genkey(password)
    f = Fernet(key)
    data = open('test.json', 'rb').read()
    open('test.json', 'wb').write(f.decrypt(data))


def main():
    dec()

main()
    

