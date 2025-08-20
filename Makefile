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
		printf "Enter default Btrfs mount point [/mnt/btrfs_test]: "; \
		read MP; \
		MP=$${MP:-/mnt/btrfs_test}; \
		echo "mount_point=$$MP" | sudo tee $(SYSCONFDIR)/btrfs-monitor/config >/dev/null; \
		echo "Saved default mount point to $(SYSCONFDIR)/btrfs-monitor/config"; \
	fi

uninstall:
	rm -f $(BINDIR)/btrfs-monitor
	# keep config unless forced by user
	@echo "Removed binary from $(BINDIR). Config at $(SYSCONFDIR)/btrfs-monitor/config was preserved."
