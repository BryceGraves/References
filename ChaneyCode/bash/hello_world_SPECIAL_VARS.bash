#!/bin/bash 


echo "The process id of this shell is $$"

echo "The name of this shell script is $0"

who > /dev/null
RES=$?

if [ ${RES} -eq 0 ]
then
    echo "The previous command was successful"
else
    echo "The previous command failed"
fi


cd DOES_NOT_EXIST > /dev/null 2>&1 
RES=$?

if [ ${RES} -eq 0 ]
then
    echo "The previous command was successful"
else
    echo "The previous command failed"
fi
