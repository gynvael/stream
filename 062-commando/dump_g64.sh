#!/bin/bash

if [ $# -eq 0 ]
then
  echo 'usage: dump_g64.sh <filename.g64>'
  exit 1
fi

NAME="$1"

mkdir -p /tmp/dumpdir/Program
dtc -p -f/tmp/dumpdir/Program/Program_ -i0 -g0 -i2 -i6 -l8 -t10
dtc -m1 -f/tmp/dumpdir/Program/Program_ -i0 -g0 -i2 -f${NAME} -k2 -i22 -l8
rm -rf /tmp/dumpdir

ls -la ${NAME}

