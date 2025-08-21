<<<<<<< HEAD
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
=======
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
SYSCONFDIR ?= /etc

all: btrfs-monitor

btrfs-monitor: main.o btrfs.o visual.o
	gcc -o btrfs-monitor main.o btrfs.o visual.o

main.o: main.c btrfs.h visual.h
	gcc -c main.c

btrfs.o: btrfs.c btrfs.h visual.h
	gcc -c btrfs.c

visual.o: visual.c visual.h
	gcc -c visual.c

clean:
	rm -f *.o btrfs-monitor

install: all
	install -d $(BINDIR)
	install -m 0755 btrfs-monitor $(BINDIR)/btrfs-monitor
	@echo "Installing config..."
	install -d $(SYSCONFDIR)/btrfs-monitor
	@if [ -f "$(SYSCONFDIR)/btrfs-monitor/config" ]; then \
		echo "Config exists at $(SYSCONFDIR)/btrfs-monitor/config"; \
	else \
		printf "Enter default Btrfs mount point [/mnt/btrfs]: "; \
		read MP; \
		MP=$${MP:-/mnt/btrfs}; \
		echo "mount_point=$$MP" | sudo tee $(SYSCONFDIR)/btrfs-monitor/config >/dev/null; \
		echo "Saved default mount point to $(SYSCONFDIR)/btrfs-monitor/config"; \
	fi

uninstall:
	rm -f $(BINDIR)/btrfs-monitor
	# keep config unless forced by user
	@echo "Removed binary from $(BINDIR). Config at $(SYSCONFDIR)/btrfs-monitor/config was preserved."
>>>>>>> 3e1d7d8631993c75b7f95299e96481fffadad091
