# slstatus version
VERSION = 1.1

# customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# flags
CPPFLAGS = -I$(X11INC) -D_DEFAULT_SOURCE -DALSA -DVERSION=\"${VERSION}\"
CFLAGS   = -march=native -funsafe-math-optimizations -ffast-math -funroll-loops -fomit-frame-pointer -fno-semantic-interposition -Ofast -flto -std=c99 -pedantic -Wall -Wextra -Wno-unused-parameter
LDFLAGS  = -flto -L$(X11LIB) -s
# OpenBSD: add -lsndio
# FreeBSD: add -lkvm -lsndio
LDLIBS   = -lX11 -lasound

# compiler and linker
CC = cc
