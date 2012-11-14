CFLAGS = -std=c99 -Wall -Wextra -Wwrite-strings -Wunreachable-code -fPIC
LDFLAGS =
PRJNAME = libzcfg
SOURCES = *.c

all:
	gcc $(CFLAGS) -o $(PRJNAME).o -c $(SOURCES) $(LDFLAGS) -include ./util.h
	gcc -shared -Wl,-soname,$(PRJNAME).so -o $(PRJNAME).so $(PRJNAME).o
	ar rcs $(PRJNAME).a $(PRJNAME).o

clean:
	rm $(PRJNAME)
