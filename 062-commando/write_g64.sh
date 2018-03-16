#!/bin/bash

if [ $# -eq 0 ]
then
  echo 'usage: write_g64.sh <filename.g64>'
  exit 1
fi

NAME="$1"

dtc -f${NAME} -w -dd1

