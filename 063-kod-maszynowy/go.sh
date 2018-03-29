#!/bin/bash
sed 's/#.*//' test.hex | xxd -r -p > test.bin
ls -la test.bin
