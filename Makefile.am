AUTOMAKE_OPTIONS = foreign subdir-objects
CC = @CC@ -Wall
CPP = @CPP@
AM_CFLAGS = $(TCDB_CFLAGS) -I$(top_srcdir)/include -I$(KERNELDIR)/include
LIBS = @LIBS@ $(TCDB_LIBS)

lib_LTLIBRARIES = libTcIPCCtrl.la
libTcIPCCtrl_la_SOURCES = src/tc-ipc-ctrl.c \
						src/ipc-dev/tc-ipc-dev.c \
						src/ipc-parser/tc-crc.c \
						src/ipc-parser/tc-package.c \
						src/ipc-parser/tc-parser-thread.c \
						src/ipc-parser/tc-parser.c \
						src/ipc-rw-control/tc-buffer.c \
						src/ipc-rw-control/tc-read-thread.c \
						src/ipc-rw-control/tc-rw-control.c \
						src/ipc-rw-control/tc-write-thread.c

libTcIPCCtrl_la_LDFLAGS = -version-info 1:1:0

include_HEADERS = include/tc-ipc-ctrl.h
