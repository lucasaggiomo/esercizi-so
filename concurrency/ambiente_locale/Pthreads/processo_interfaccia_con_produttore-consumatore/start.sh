#!/bin/bash

ipcs -q | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -q {}
make clean
make
clear
./main
