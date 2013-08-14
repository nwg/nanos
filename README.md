Overview
========

This is a toy x86 OS. It consists of a boot loader, a 64-bit long-mode kernel,
and two statically located user programs. The kernel runs the two programs
simultaneously, switching between them using the PIT. A software interrupt is
provided for user programs at 30h, which currently just prints a simple message to
the screen.

![Sample screenshot](https://github.com/nwg/simpleos/raw/master/img/sample1.png)

Instructions (Mac OS X):


1) sudo port install qemu x86_64-elf-gcc nasm

Note that the nasm included with the Xcode command-line tools is extremely
outdated and will not build this project properly.

2) make (the kernel is a flat binary called bigboot)

3) make run (to test in qemu)


Debugging Instructions (Mac OS X):

The bochs installation from macports does not have x86_64 support, you will
need to compile your own

If on mountain lion, first install XQuartz from
http://xquartz.macosforge.org/landing/ This will give you an X server.

```shell
sudo port install gtk2
curl -OJL http://sourceforge.net/projects/bochs/files/latest/download?source=files
tar xvf bochs-$VERSION.tar.gz
cd bochs-$VERSION
CFLAGS=-I/opt/local/include LDFLAGS=-L/opt/local/lib ./configure \
--enable-all-optimizations --enable-debugger  --enable-cdrom --with-x11 \
--enable-x86-64
make
sudo make install
```

Once everything is set up, you should be able to issue a "make debug" in the
source directory and see the bochs gui debugger.
