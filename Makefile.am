## Copyright (C) 2017  Trevor Woerner <twoerner@gmail.com>

SUBDIRS =
DIST_SUBDIRS = cfg

AM_CPPFLAGS = -Wall -Werror -Wextra -Wreturn-type -fno-strict-aliasing -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE $(LIBUSB1_CFLAGS)

bin_PROGRAMS = rkdeveloptool
rkdeveloptool_SOURCES = main.cpp \
	crc.cpp DefineHeader.h Endian.h DefineHeader.h Property.hpp \
	RKBoot.cpp RKBoot.h \
	RKComm.cpp RKComm.h \
	RKDevice.cpp RKDevice.h \
	RKImage.cpp RKImage.h \
	RKLog.cpp RKLog.h \
	RKScan.cpp RKScan.h \
	gpt.h
rkdeveloptool_LDADD = $(LIBUSB1_LIBS)

clean-local::
	rm -fr log
