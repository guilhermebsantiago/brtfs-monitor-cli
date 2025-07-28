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
