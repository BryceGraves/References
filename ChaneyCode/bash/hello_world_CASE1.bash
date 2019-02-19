#!/bin/bash

VALUE=5
case $VALUE in
    0)
        echo "A zero"
        ;;
    5)
        echo "A 5"
        ;;
    10)
        echo "A 10"
        ;;
    *)
        echo "A strange value"
        ;;
esac
