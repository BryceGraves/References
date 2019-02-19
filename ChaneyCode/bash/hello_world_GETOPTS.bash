#!/bin/bash

VAL=-1
STR="default"

function show_help()
{
    echo "This is the help function"
    exit
}

while getopts "hv:s:" OPT
do
    case "${OPT}" in
        h)
            show_help
            ;;
        v)
            VAL=${OPTARG}
	    #echo "    ${VAL}"
	    ;;
	s)
	    STR=${OPTARG}
	    ;;
	*)
	    echo "Unknown command line option ${OPTARG}"
	    ;;
    esac
done


echo "The value of VAL is ${VAL}"
echo "The value of STR is ${STR}"
