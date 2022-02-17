#!/bin/bash

count=$#
codefile=$1
exefile=$2

if [ $count -eq 2 -a -f "$codefile" ]
then
	gcc "$codefile" -o "$exefile" && exec "./$exefile"
else
	{
		echo "You should use 2 arguments"
		echo "First is code file"
		echo "Second is exe file"
	} >&2
fi
