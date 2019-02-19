#!/usr/bin/python

import glob
import os
import pwd
import sys
import getopt

cmdOptions = "hmu:"
uid = -1
userName = ''

try:
    options, args = getopt.getopt(sys.argv[1:], cmdOptions)
except getopt.GetoptError as err:
    print str(err)  # will print something like "option -a not recognized"
    exit(1)

for option, argument in options:
    if option == "-h":
        print "Usage: " + sys.argv[0] + " [" + cmdOptions + "]"
        exit(0)
        pass
    elif option == "-m":
        uid = os.getuid()
    elif option == "-u":
        userName = argument
        try:
            uid = pwd.getpwnam(userName).pw_uid
        except KeyError:
            print "Cannot find user %s" % userName
            exit(2)
    else:
        print "bad option"
        exit(3)

for filename in glob.glob('/proc/[1-9]*/status'):
    if uid != -1:
        try:
            status = os.stat(filename)
        except IOError:
            # If we cannot stat the file, just move on.
            continue
        fuid = status.st_uid
        if uid != fuid:
            continue
    # It would be good to protect the open with a try block.
    # the files in /proc can be very transient.
    try:
        fp = open(filename)
    except IOError:
        # If we cannot open the file, just move on.
        continue
    try:
        lines = fp.readlines()
        fp.close()
    except IOError:
        # If we cannot read the file, just move on.
        try:
            fp.close()
        except IOError:
            pass
            continue
    for line in lines:
        line = line.rstrip()
        if line[:5] == "Name:":
            print "%-28s" % line,
        elif line[:6] == "State:":
            print "  %-20s" % line,
        elif line[:8] == "Threads:":
            print "  %s" % line
            # I could just break out of the loop at this point.
