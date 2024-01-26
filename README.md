Overview
========

This is an x86_64 operating system written from scratch in C and assembler.  It
consists of a boot loader, a 64-bit long-mode kernel, and a round-robin
scheduler. The userspace programs are loaded directly from disk as there is no
filesystem. The kernel includes a vga 80x25 terminal. User progs take
argc/argv. There is a basic userspace shell. Newlib is used as a C library for
userspace. I've also ported to 64-bit and simplified an ATA driver called
MINDRV, it's hardwired to file descriptor three in userspace and supports both DMA-
and PIO-based disk I/O.

Most tutorials and resources only cover 32-bit intel systems. I took the time
to build everything with 64-bit support from the ground up. I relied a lot on
#osdev, too. You might check this project out if you are looking for a simple
functioning example of 64-bit intel OS, as i found many resources incomplete or
incorrect when i built it.

Syscall interface includes:

- sbrk
- exit
- sleep
- read (stdin and ata disk)
- write (stdout only)
- spawn (run static user prog from startup image)
- wait
- getpid

![Sample screenshot 1](https://github.com/nwg/simpleos/raw/master/img/sample2.png)
![Sample screenshot 2](https://github.com/nwg/simpleos/raw/master/img/sample3.png)

Instructions (Mac OS X)
========================

1. sudo port install qemu x86_64-elf-gcc nasm
Note that the nasm included with the Xcode command-line tools is extremely
outdated and will not build this project properly.
1. make (the kernel is a flat binary called bigboot)
1. make run (to test in qemu)


Debugging Instructions (Mac OS X)
=================================

The bochs installation from macports does not have x86_64 support, you will
need to compile your own

If on mountain lion, first install XQuartz from
http://xquartz.macosforge.org/landing/ This will give you an X server.

Then, install some prequisites and build bochs and an elf gdb

```shell
sudo port install gtk2
curl -OJL http://sourceforge.net/projects/bochs/files/latest/download?source=files
tar xvf bochs-$VERSION.tar.gz
cd bochs-$VERSION
CFLAGS=-I/opt/local/include LDFLAGS=-L/opt/local/lib ./configure \
--enable-all-optimizations --enable-cdrom --with-x11 \
--enable-x86-64 --enable-debugger  
make
sudo make install
sudo install bochs /usr/local/bin/bochs-native-debugger

# And if you'd like gdb stub support
CFLAGS=-I/opt/local/include LDFLAGS=-L/opt/local/lib ./configure \
--enable-all-optimizations --enable-cdrom --with-x11 \
--enable-x86-64 --enable-gdb-stub
make
sudo install bochs /usr/local/bin/bochs-gdb-stub

sudo port install gmp libelf
curl -OJL http://ftp.gnu.org/gnu/gdb/gdb-7.6.tar.gz
tar xvf gdb-7.6.tar.gz
cd gdb-7.6
mkdir build
cd build
../configure  --prefix=/usr/local --program-prefix=x86_64-elf-
--with-gmp=/opt/local --with-libelf=/opt/local
--with-build-libsubdir=/opt/local --target=x86_64-pc-linux-gnu
make
sudo make install

```

Once everything is set up, you should be able to issue a "make bochs-native" in
the source directory and see the bochs debugger console. If you compiled gdb
support above, you can issue "make bochs-gdb" and connect by running the newly
installed x86_64-elf-gdb from the main os source checkout directory (using
provided .gdbinit).

---

Licensed under the Unlicense. You should be able to do whatever you want with
this without having to give credit.
