#!/bin/bash

count=$#
outputfile=$1
directory=$2
extension=$3

if [ $count -eq 3 -a -d "$directory" ]
then
	find $directory -name "*.$extension" | sort -f > $outputfile
else
	{
		echo "You should use 3 arguments"
		echo "First is output file name"
		echo "Second is target directory"
		echo "Third is file extension"
	} >&2
fi
