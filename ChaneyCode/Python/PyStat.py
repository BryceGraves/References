#!/usr/bin/python
#
# R Jesse Chaney
# 

#
# Import into Python modules we'll be using in this script.
#
import sys
import stat
import time
import os
import pwd
import grp


# This is what the output from the Python program mystat looks like.
#
# File: Makefile
#   File type:                regular file
#   I-node number:            1691871
#   Device ID number:         30
#   Mode:                     100640 (octal)
#   Link count:               1
#   Ownership Ids:            UID=50255   GID=10993
#   Preferred I/O block size: 131072 bytes
#   File size:                1250 bytes
#   Blocks allocated:         16
#   Last status change:       Wed Jan  7 06:41:21 2015
#   Last file access:         Wed Jan  7 06:41:24 2015
#   Last file modification:   Wed Jan  7 06:41:21 2015

def show_file_stat():
    # Loop through the files listed on the command line.
    for f in sys.argv[1:]:

        #print f , "\n"

        status = os.lstat(f)
        mode = status.st_mode

        print "File: %s" % f
        print "  File type:               ",
        if stat.S_ISDIR(mode):
            print 'directory'
	elif stat.S_ISLNK(mode):
            realfile = os.readlink(f)
            rstatus = os.lstat(realfile)
            if stat.S_ISDIR(rstatus.st_mode):
                dir_char = "/"
            else:
                dir_char = ""
            print 'symlink -> ' + realfile + dir_char
	elif stat.S_ISCHR(mode):
            print 'character device'
	elif stat.S_ISBLK(mode):
            print 'block device'
	elif stat.S_ISFIFO(mode):
            print 'FIFO/pipe'
	elif stat.S_ISREG(mode):
            # Since this is the most common, I should make this
            # first.
            print 'regular file'
	else:
            print 'unknown'  # yikes, this is strange

        print '*****', status

        # All of these fields look like the same ones we used
        # in the C example of the same thing.
        print '  I-node number:            %d' % status.st_ino
        print '  Device ID number:         %d' % status.st_dev
        print '  Mode:                     %o (octal)' % status.st_mode
        print '  Link count:               %d' % status.st_nlink
        user_name =  pwd.getpwuid(status.st_uid).pw_name
        group_name = grp.getgrgid(status.st_gid).gr_name
        print '  Ownership Ids:            UID=%5d' % status.st_uid \
            + " (" + user_name + ")" \
            , '  GID=%5d' % status.st_gid + " (" + group_name + ")"
        print '  Preferred I/O block size: %d bytes' % status.st_blksize
        print '  File size:                %d bytes' % status.st_size
        print '  Blocks allocated:         %d' % status.st_blocks

        # I am being a bit lazy here, I'm not formatting the time
        # quite right.  The width of the day of the month should
        # always be 2 and the value should be right justified, but
        # I'm not doing that here.
        t = time.ctime(status.st_ctime).split()
        print '  Last status change:       %s' % " ".join(t)
        t = time.ctime(status.st_atime).split()
        print '  Last status change:       %s' % " ".join(t)
        t = time.ctime(status.st_mtime).split()
        print '  Last status change:       %s' % " ".join(t)

if __name__ == '__main__':
	show_file_stat()
