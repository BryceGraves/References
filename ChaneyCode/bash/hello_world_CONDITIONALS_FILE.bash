#!/bin/sh

FILE="hello_world.bash"
if [ -e $FILE ]
then
    echo "The file $FILE exists"
    if [ -s $FILE ]
    then
        echo "  The file $FILE has a size greater than zero"
    fi
else
    echo "The file $FILE does not exist"
fi
