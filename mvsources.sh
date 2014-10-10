#!/bin/bash

sources=$(ls *.h);

for file in $sources
do
    name=$(expr $file : '\(.*\)\.' \| $file);
    mkdir $name;
    mv "$name.c" $name
    mv "$name.h" $name
done
