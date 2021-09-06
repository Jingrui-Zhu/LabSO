#!/bin/bash

if test $# -lt 2;
	then
		echo "Usage: $0 <integer> <file_name.txt>"
		exit 1
fi

make -f makecalc
make -f makemem

INTEGER="$1"
FILE_NAMe="$2"

sleep 1s

./memorizzatore&./calcolatore $INTEGER

#./calcolatore $INTEGER&./memorizzatore 


