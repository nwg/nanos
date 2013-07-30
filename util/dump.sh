#!/bin/bash

exec x86_64-elf-objdump  --architecture=i386  -b binary -D "$@"
