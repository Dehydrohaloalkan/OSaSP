#!/bin/bash

count=$#
directory=$1
outputfile=$2

if [ $count -eq 2 -a -d "$directory" ]
then
	
	for i in $(find $directory -type d)
	do
		path=$(realpath $i)
		size=$(du -bs $i | cut -f 1)
		count=$(find $i -maxdepth 1 -type f | wc -l)

		echo -e "$path \t $size \t $count" >> $outputfile
	done

else
	{
		echo "You should use 2 arguments"
		echo "First is target directory"
		echo "Second is output file name"
	} >&2
fi