Overview
========

This is a toy x86 OS. It consists of a boot loader, a 64-bit long-mode kernel,
and two statically located user programs. The kernel runs the two programs
simultaneously, switching between them using the PIT. A software interrupt is
provided for user programs at 30h, which currently just prints a simple message to
the screen.

![Sample screenshot](https://github.com/nwg/simpleos/raw/master/img/sample1.png)

Instructions (Mac OS X):

1) Download the latest nasm from http://www.nasm.us/pub/nasm/releasebuilds/
  Note that the nasm included with the Xcode command-line tools is quite old and will not
  build this project properly.

2) sudo port install qemu

3) make
