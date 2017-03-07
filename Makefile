# Simple Makefile for RK Flash Tool

CC	= g++
LD	= $(CC)
CXXFLAGS	= -O2 -Wall -fno-strict-aliasing -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -I/usr/local/include/libusb-1.0 
LDFLAGS	= -L/usr/local/lib -Wl,-Bstatic -lusb-1.0 -Wl,-Bdynamic -lrt -lpthread


PROGS	= $(patsubst %.cpp,%.o, $(wildcard *.cpp))

rkDevelopTool: $(PROGS)
	$(CC) $(CXXFLAGS) $^ -o rkDevelopTool $(LDFLAGS)

install: $(PROGS)
	install -d -m 0755 /usr/local/bin
	install -m 0755 ./rkDevelopTool /usr/local/bin

clean:
	rm $(PROGS) ./rkDevelopTool

uninstall:
	cd /usr/local/bin && rm -f ./rkDevelopTool


