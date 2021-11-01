#!/usr/bin/env bash
gcc -o rv64asm.so rv64asm.c $(yed --print-cflags) $(yed --print-ldflags)
