#!/usr/bin/python
#
# R Jesse Chaney
# 

#
# Import into Python modules we'll be using in this script.
#
import subprocess
import shlex

# Define a new function called ls_sp() (for ls using the subprocess module)
def ls_sp():
        
        # We specify the ls command and some options:
        #   -l     use a long listing format
        #   -a     do not ignore entries starting with .
        #   -t     sort by modification time
        #   -r     reverse order while sorting
        #   -n     like -l, but list numeric user and group IDs
        #
	# Either of the formats for the ls options below works. Try
        # both out.
        #command = 'ls -l -a -t -r -n'
        command = 'ls -latrn'
        
	# The shlex() function will split the string into a list of strings.
        # It is really handy for parsing command lines.
        command_sequence = shlex.split(command)
        print "the command sequence is: %s" % command_sequence

	# Open a new process (the Popen() call) and connect a pipe to the
        # standard out (stdout) new process. We will read the results of
        # the commands from the pipe connected to stdout.
        # 
	# Python deals with creation of the new process, connecting a pipe
        # to stdout, and closing the process whe you are done.
        ls = subprocess.Popen(command_sequence, stdout = subprocess.PIPE)

	# Read the results of the command as a list of strings.
        ls_out = ls.stdout.readlines()
        #print "the result from the readlines is: %s" % ls_out

	# Loop through the list of strings and print them. Do not
        # move to a new line for each print statement. That is what
        # the trailing comma does for you. Since the returned lines
        # from the ls command already contain newline characters,
        # they are printed from that.
        for line in ls_out:
                print line,

# When you start a Python script, the Python file you identify on the command line
# to Python contains a global name called __name__. The value of that variable is
# '__main__'.
if __name__ == '__main__':
        ls_sp()
