# Simple Makefile for RK Flash Tool

CC	= g++
LD	= $(CC)
CXXFLAGS= -O2 -Wall -fno-strict-aliasing -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -I/usr/include/libusb-1.0 
LDFLAGS	= -L/usr/lib -Wl,-Bstatic -lusb-1.0 -Wl,-Bdynamic -ludev -lrt -lpthread


PROGS	= $(patsubst %.cpp,%.o, $(wildcard *.cpp))

rkdeveloptool: $(PROGS)
	$(CC) $(CXXFLAGS) $^ -o rkdeveloptool $(LDFLAGS)

install: $(PROGS)
	install -d -m 0755 /usr/local/bin
	install -m 0755 ./rkdeveloptool /usr/local/bin

clean:
	rm $(PROGS) ./rkdeveloptool

uninstall:
	cd /usr/local/bin && rm -f ./rkdeveloptool


