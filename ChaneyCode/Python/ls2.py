#!/usr/bin/python
#
# R Jesse Chaney
# 

#
# Import into Python modules we'll be using in this script.
#
import subprocess
import os
import stat
import time

def convert_file_type(mode):
	p = ''
	# If the mode indicats that the file is a directory, make the
	# first letter of the permission string a 'd'
	if stat.S_ISDIR(mode):
		p = 'd'
	elif stat.S_ISLNK(mode):
		# If the mode indicates that the file is a symbolic link, make
		# the first letter of the permission string a 'l'
		p = 'l'
	elif stat.S_ISCHR(mode):
		# If the mode indicates that the file is a symbolic link, make
		# the first letter of the permission string a 'l'
		p = 'c'
	elif stat.S_ISBLK(mode):
		# If the mode indicates that the file is a symbolic link, make
		# the first letter of the permission string a 'l'
		p = 'b'
	elif stat.S_ISFIFO(mode):
		# If the mode indicates that the file is a symbolic link, make
		# the first letter of the permission string a 'l'
		p = 'p'
	elif stat.S_ISSOCK(mode):
		# If the mode indicates that the file is a symbolic link, make
		# the first letter of the permission string a 'l'
		p = 's'
	else:
		# Make the first letter of the permission string a '-'
		p = '-'

	return p

# A function to convert the integer value of file mode (permissions) into
# something more human friendly. We want to show something like this for
# the file permissions 'drwxrwx---'
#
def convert_perms(mode):  # <<--- When you create a function, you need the :
	# We use the string p to build up the text representation of the
	# permissions of mode.
	p = convert_file_type(mode)

	# If the file is set for read permission for the owner, add a 'r'
	# to the permission string.
	if mode & stat.S_IRUSR:
		p += 'r'
	else:
		p += '-'

        # If the file is set for write permission for the owner, add a 'w'
        # to the permission string.
        if mode & stat.S_IWUSR:
                p += 'w'
        else:
                p += '-'

        # If the file is set for execute permission for the owner, add a 'x'
        # to the permission string.
        if mode & stat.S_IXUSR:
                p += 'x'
        else:
                p += '-'
	
        # If the file is set for read permission for the group, add a 'r'
        # to the permission string.
        if mode & stat.S_IRGRP:
                p += 'r'
        else:
                p += '-'

        # If the file is set for write permission for the group, add a 'w'
        # to the permission string.
        if mode & stat.S_IWGRP:
                p += 'w'
        else:
                p += '-'
	
        # If the file is set for execute permission for the group, add a 'x'
        # to the permission string.
        if mode & stat.S_IXGRP:
                p += 'x'
        else:
                p += '-'

        # If the file is set for read permission for other, add a 'r'
        # to the permission string.
        if mode & stat.S_IROTH:
                p += 'r'
        else:
                p += '-'

        # If the file is set for write permission for other, add a 'w'
        # to the permission string.
        if mode & stat.S_IWOTH:
                p += 'w'
        else:
                p += '-'

        # If the file is set for execute permission for other, add a 'x'
        # to the permission string.
        if mode & stat.S_IXOTH:
                p += 'x'
        else:
                p += '-'

	p += '.'

	# Return our newly created string for the permission bits.
	return p

# Create a new function called ls_os (for emulate the ls command usin Python os
# module. The function has an optional parameter called path. If the function
# is called with no parameter, assume the arugument value is '.' (the current
# working directory.
def ls_os(path = '.'):

	# Using the os.listdir function, fetch a list of strings containing all
	# the directory entries for the given path.
	files = os.listdir(path)

	# We happen to know that both '.' and '..' are exculded from the list
	# of directory entries returned from the os.listdir() call. So, we
	# append them into the list of directory entries. By default, the
	# .split() method will split the list on spaces.
	files += '. ..'.split()

	# We are going to sort the list of files into a new list, We'll
	# start a new empty list.
	sorted_files = []

	# Loop through the list of directory entries.
	for f in files:
		#print ">>>path: ", path , "  file: ", f

		# The os.lstat() function returns lots of information the file
		# system tracks for each file. We want to use lstat() not just
		# stat() so that we return information about symbolic links.
		# file. (https://docs.python.org/2.6/library/os.html#os.stat)
		status = os.lstat(path + '/' + f)

		# We are going to pull out several pieces of information about
		# each file and keep trck of them.
		# The mode is the integer that represents the permissions
		# on the file.
		perms = status.st_mode

		# The user id and group id.
		uid = status.st_uid
		gid = status.st_gid

		# The file size, in bytes.
		size = status.st_size

		# The file modification time. This is returned as a floating
		# point value. Convert it to an integer.
		mtime = status.st_mtime

		# The number of hard links for a file.
		links = status.st_nlink

		# Create a new list with the items we just pulled out
		# the the file. We could create a dictionary with keys that
		# would be a bit easier to read, but for this, a list is fine.
		file_mdata = [mtime, perms, uid, gid, size, f, links]

		# Append the new list into our sorted_list variable (which is
		# not yet sorted). Notice that the sorted_list is a list of lists.
		sorted_files.append(file_mdata)
	
	#print sorted_files
	# Sort the list of lists. This sort will be done in place. It does
	# not create a new list, rather this will modify the existing list.
	# We happend to know that when a list of lists is sorted, it is sorted
	# by the fist element in the inner lists. We exploit that information.
	sorted_files.sort()

	# We now need to loop through the sorted list and print the information.
	for f in sorted_files:
		# mtime  perms  uid  gid  size  file_name links
		#   0      1     2    3     4       5       6

		ctime = time.ctime(f[0]).split()
				
		# Before we print the permissions, we need to convert it from
		# an integer into the string, something like `-rw-rw----`. The
		# convert_perms() function, which we define above, does this
		# for us. We also need to convert the time representation from
		# a floating point value into a human readible string. The time.ctime()
		# will do that very thing. 
		#
		# Notice that the line with the print ends with a comma. Also 
		# notice the C style syntax that can be used within the Python 
		# print statement.
		# https://docs.python.org/release/2.6.7/tutorial/inputoutput.html#old-string-formatting
		print convert_perms(f[1]), '%2d' % f[6], f[2], f[3], '%4s' % f[4] \
		    , ctime[1], "%2s" % ctime[2] \
		    , ":".join(ctime[3].split(":")[0:2]),

		# When a file is a symbolic link, we want to print the little
		# pointer and show to what what link points. So, we check
		# to see if the file is a link using the os.path.islink()
		# function.
		if os.path.islink(f[5]):
			# If the file is a symbolic link, we print the file name,
			# the little ' -> ' pointer, and use the os.readlink()
			# function to have the operating system return the name
			# of the file/directory to which the link points.
			print f[5] + ' -> ' + os.readlink(f[5])
		else:
			print f[5]

if __name__ == '__main__':
	ls_os()
