#!/bin/bash

for FILE in /proc/[1-9]*/status
do
    echo "Status file $FILE"
done
