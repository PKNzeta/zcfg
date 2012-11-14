CFLAGS = -g -DDEBUG -std=c99 -Wall -Wextra -Wwrite-strings -Wunreachable-code
LDFLAGS = -static -L.. -lzcfg
PRJNAME = zcfg_exemple
SOURCES = *.c

all:
	gcc $(CFLAGS) -o $(PRJNAME) $(SOURCES) $(LDFLAGS) -include ./util.h

clean:
	rm $(PRJNAME)
