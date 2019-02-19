#!/usr/bin/python

import sys


if __name__ == '__main__':
    file_name = sys.argv[1]

    f = open(file_name, 'r')

    txt = f.read()

    str = ''.join(txt.split())

    base = 16
    print txt
    print
    print str
    print
    print str[2], int(str[2], base)

