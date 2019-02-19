#!/bin/bash

STR1="Hello World"
STR2="This is within double ${STR1} quotes"
STR3='This is within single ${STR1} quotes'

echo $STR1
echo $STR2
echo $STR3
