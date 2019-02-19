#!/bin/bash

VALUE=0
while [ $VALUE -le 10 ]
do
    echo "The value is $VALUE"
    VALUE=$(( VALUE + 1 ))
done
