PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin

.PHONY: all clean install

all: btrfs-cli

btrfs-cli: main.o btrfs.o visual.o
	gcc -o btrfs-cli main.o btrfs.o visual.o

main.o: main.c btrfs.h visual.h
	gcc -c main.c

btrfs.o: btrfs.c btrfs.h visual.h
	gcc -c btrfs.c

visual.o: visual.c visual.h
	gcc -c visual.c

install: btrfs-cli
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 btrfs-cli $(DESTDIR)$(BINDIR)/btrfs-cli

clean:
	rm -f *.o btrfs-cli
