C = cc

SOURCES = src/md5file.c \
      src/pngrw.c \
      src/basename.c \
	  src/pngfy.c

CFLAGS = -Wall -O2
LDFLAGS = -lcrypto -lpng


OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = pngfy

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(C) $(OBJECTS) -o $@ $(CFLAGS) $(LDFLAGS)

cleanall: clean

clean:
	-rm $(EXECUTABLE) $(OBJECTS)

