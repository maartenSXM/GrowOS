#!/bin/bash

for prj in projects/*.mk; do
   printf "$0: invoking \"make PRJ=$prj\" $@\n";
   make PRJ=$prj $@;
done

exit
