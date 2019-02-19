#!/bin/bash

COUNT=1
echo "COUNT starts off as equal $COUNT"

COUNT=$((COUNT + 1))
echo "COUNT plus 1 = $COUNT"

COUNT=$((COUNT * 2))
echo "COUNT times 2 = $COUNT"

OTHER=6
COUNT=$((COUNT + OTHER))
echo "COUNT with $OTHER added = $COUNT"
