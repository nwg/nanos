#!/bin/bash

exec x86_64-elf-objdump  --architecture=i386 -M x86-64 -b binary -D "$@"
