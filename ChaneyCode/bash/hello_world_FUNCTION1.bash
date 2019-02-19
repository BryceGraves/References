#!/bin/bash

print_something()
{
    echo "  This is from the inside of a function"
}

echo "not in a function"
print_something
echo "not in a function"
print_something
echo "not in a function"
