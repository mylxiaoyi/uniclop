PWD = $(shell pwd)
INSTALL_DIR = $(PWD)/../lib
INCLUDE_DIR = $(PWD)/../include
CC = g++
LIBTOOL = libtool
INSTALL = install
CFLAGS = -c -g -O3 -DNDEBUG -I$(INCLUDE_DIR) `pkg-config opencv --cflags`
LDFLAGS = `pkg-config opencv --libs` -rpath $(INSTALL_DIR)
SOURCES = detector.cpp integral.cpp keypoint.cpp
OBJECTS = $(SOURCES:.cpp=.lo)
LIBNAME = libwgdetect

all: $(LIBNAME).so

$(LIBNAME).so: $(OBJECTS)
	$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) $(OBJECTS) -o $(LIBNAME).la
	$(LIBTOOL) --mode=install $(INSTALL) $(LIBNAME).la $(INSTALL_DIR)

$(OBJECTS): %.lo: %.cpp
	$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) $<

clean:
	rm -rf $(OBJECTS) $(SOURCES:.cpp=.o) .libs $(INSTALL_DIR)/* $(LIBNAME).la
