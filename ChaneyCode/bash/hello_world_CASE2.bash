#!/bin/bash

VALUE="A"
case $VALUE in
    "a" | "A" )
        echo "An A or a was seen"
        ;;
    "b" | "B" )
        echo "A B or b was seen"
        ;;
    *)
        echo "A strange value $VALUE"
        ;;
esac
