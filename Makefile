# Makefile for XGlobe
#
# $Id: Makefile,v 1.2 2000/06/15 09:45:34 espie Exp $

####### Installation directory

XGLOBE_DIR     = /usr/local
XGLOBE_LIB_DIR = $(XGLOBE_DIR)/lib/xglobe
XGLOBE_BIN_DIR = $(XGLOBE_DIR)/bin

####### Library directories - you may need to modify these

X11_INCLUDE_DIR = /usr/X11R6/include
QT_INCLUDE_DIR  = $(QTDIR)/include

QT_LIB_DIR      = $(QTDIR)/lib
X11_LIB_DIR     = /usr/X11R6/lib


####### Compiler and tools

CPP     = g++
LINK    = g++
MOC     = moc
INSTALL = install

####### compile and link options

CFLAGS       = $(INCLUDE_DIRS) -DXGLOBE_LIB_DIR=\"$(XGLOBE_LIB_DIR)\" \
$(WITH_QIMGIO) $(RANDOM_METHOD) $(OPTIONS)
OPTIONS=-O3 -Wall -fno-exceptions -funroll-loops $(COPTS)
COPTS=

LFLAGS       =

INCLUDE_DIRS = -I$(QT_INCLUDE_DIR) -I$(X11_INCLUDE_DIR)

LIB_DIRS     = -L$(QT_LIB_DIR) -L$(X11_LIB_DIR)

LIBS         = -lX11 -lqt -lm
# If you want to use the QImageIO lib (to support jpg and png maps) use the
# next two lines and comment the one above
#WITH_QIMGIO  = -DWITH_QIMAGEIO
RANDOM_METHOD= -DUSE_RANDOM
#LIBS         = -lX11 -lqt -lm -lqimgio
# If the linker complains about unresolved references try this:
#LIBS         = -lX11 -lqt -lm -lqimgio -lpng -lz -ljpeg

####### Target

TARGET	= xglobe

####### Files

OBJECTS    = main.o earthapp.o renderer.o desktopwidget.o sunpos.o moonpos.o \
 markerlist.o random.o stars.o compute.o file.o
SRCMOC     = earthapp.moc
MAPFILES   = map.bmp
MARKERFILE = xglobe-markers

INSTALLED_DATA_FILES = $(XGLOBE_LIB_DIR)/map.bmp \
                       $(XGLOBE_LIB_DIR)/xglobe-markers

####### Implicit rules

.SUFFIXES: .cpp

.cpp.o:
	$(CPP) -c $(CFLAGS) -o $@ $<

####### Build rules

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) $(LFLAGS) $(LIB_DIRS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	-rm -f $(OBJECTS) $(SRCMOC)
	-rm -f *~ core

install:
	@echo Installing XGlobe executable in $(XGLOBE_BIN_DIR)
	@test -d $(XGLOBE_BIN_DIR) || mkdir $(XGLOBE_BIN_DIR)
	$(INSTALL) -m 755 $(TARGET) $(XGLOBE_BIN_DIR)
	@echo Installing XGlobe data files in $(XGLOBE_LIB_DIR)
	@test -d $(XGLOBE_LIB_DIR) || mkdir $(XGLOBE_LIB_DIR)
	$(INSTALL) -m 644 $(MARKERFILE) $(XGLOBE_LIB_DIR)
	$(INSTALL) -m 644 $(MAPFILES) $(XGLOBE_LIB_DIR)
	@echo Done.

uninstall:
	@echo Removing XGlobe executable from $(XGLOBE_BIN_DIR) ...
	@rm $(XGLOBE_BIN_DIR)/$(TARGET)
	@echo Removing XGlobe data files from $(XGLOBE_LIB_DIR) ...
	@rm $(INSTALLED_DATA_FILES)
	@rmdir $(XGLOBE_LIB_DIR)
	@echo XGlobe removed.

####### Dependencies

renderer.o: renderer.cpp renderer.h config.h marker.xpm file.h

earthapp.o: earthapp.cpp earthapp.h renderer.h config.h desktopwidget.h \
 moonpos.h

main.o:   main.cpp earthapp.h config.h

desktopwidget.o: desktopwidget.cpp desktopwidget.h

sunpos.o: sunpos.cpp sunpos.h

moonpos.o: moonpos.cpp moonpos.h

markerlist.o: markerlist.cpp markerlist.h

file.o: file.cpp file.h

earthapp.cpp: earthapp.moc;
earthapp.moc: earthapp.h
	$(MOC) -o $@ earthapp.h
