#!/bin/bash

STR1=`date`
STR2=`last | head`

echo "Today's date is $STR1"
echo -e "The 10 most recent users to log in are\n$STR2"
