#!/bin/bash

find -type f -name "*.h" -or -name "*.c" -or -name "*m16" -or -name "*m8" | xargs cat | wc -l

