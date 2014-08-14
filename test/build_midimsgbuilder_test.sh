#!/bin/bash
gcc -I../inc ../src/*.c ./mm_midimsgbuilder_test.c -ggdb3 \
    -o ./mm_midimsgbuilder_test.elf
