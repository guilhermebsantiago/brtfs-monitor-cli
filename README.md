# Btrfs Monitor CLI

A small C CLI that inspects a Btrfs mount point to display subvolumes, snapshots, disk usage, and features like compression and RAID.

---

## Features

- Detects whether a path is Btrfs
- Shows subvolumes and snapshots with colored usage bars
- Detects compression flags
- Detects RAID layout (best-effort parsing of `btrfs filesystem df`)
- Configurable default mount point via flag, environment variable, or config file

---

## License

MIT License. See `LICENSE`.

---

## Requirements

- Linux with Btrfs support
- `btrfs-progs`
- Build tools: `gcc`, `make`

Install dependencies on Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y gcc make btrfs-progs
```

---

## Build

```bash
make
```

This generates the binary `btrfs-monitor` in the project root.

---

## Installation

Quick install (interactive):

```bash
sudo make install
```

This will:
- Install `btrfs-monitor` to `/usr/local/bin`
- Ask for a default mount point and save it to `/etc/btrfs-monitor/config`

Uninstall:

```bash
sudo make uninstall
```

---

## Configuration

Resolution order for the mount point:

1. CLI argument: `btrfs-monitor /path/to/mount`
2. Environment variable: `BTRFS_MONITOR_MOUNTPOINT=/path`
3. Config files (first found wins):
   - `/etc/btrfs-monitor/config`
   - `$XDG_CONFIG_HOME/btrfs-monitor/config`
   - `$HOME/.config/btrfs-monitor/config`

Configuration format:

```
mount_point=/mnt/btrfs_test
```

Fallback default if nothing is set: `/mnt/btrfs_test`.

---

## Debian package (.deb)

Build a Debian package locally:

```bash
chmod +x scripts/make-deb.sh
VERSION=0.1.0 ./scripts/make-deb.sh
sha256sum dist/*.deb > dist/sha256sums.txt
```

The package will be generated under `dist/` (e.g., `dist/btrfs-monitor_0.1.0.deb`). Install it with:

```bash
sudo apt install ./dist/btrfs-monitor_0.1.0.deb
```

To publish for download, attach the `.deb` to a GitHub Release and share the release URL.

---

## Usage

Run with the configured default:

```bash
sudo btrfs-monitor
```

Run on a different mount point:

```bash
sudo btrfs-monitor /mnt/btrfs_raid
```

What you will see:

- Subvolumes and snapshots with colored usage bars
- Compression status
- RAID detection (if present)
- Total volume usage

---

## Create a demo Btrfs setup (optional)

```bash
fallocate -l 2G ~/btrfs-volume.img
sudo mkfs.btrfs ~/btrfs-volume.img
sudo mkdir -p /mnt/btrfs_test
sudo mount ~/btrfs-volume.img /mnt/btrfs_test

sudo btrfs subvolume create /mnt/btrfs_test/@root
sudo btrfs subvolume create /mnt/btrfs_test/@home
sudo btrfs subvolume create /mnt/btrfs_test/@var

sudo fallocate -l 1G /mnt/btrfs_test/@home/data.img
sudo fallocate -l 500M /mnt/btrfs_test/@var/log.img
```

Enable compression (zstd):

```bash
sudo umount /mnt/btrfs_test
sudo mount -o compress=zstd ~/btrfs-volume.img /mnt/btrfs_test
```

Create a RAID1 demo (optional):

```bash
fallocate -l 1G ~/btrfs-disk1.img
fallocate -l 1G ~/btrfs-disk2.img
sudo mkfs.btrfs -d raid1 -m raid1 ~/btrfs-disk1.img ~/btrfs-disk2.img
sudo losetup -fP ~/btrfs-disk1.img
sudo losetup -fP ~/btrfs-disk2.img
sudo btrfs device scan
sudo mkdir -p /mnt/btrfs_raid
sudo mount /dev/loop0 /mnt/btrfs_raid
```

---

## Project layout

- `main.c`: entry point and configuration resolution
- `btrfs.c` / `btrfs.h`: Btrfs feature detection and listing helpers
- `visual.c` / `visual.h`: colorized progress bars
- `Makefile`: build and install targets

---

## Clean

```bash
make clean
```

---

## Notes

- The binary uses `sudo`-only commands (e.g., `btrfs subvolume list`) in some flows; run with privileges if you get permission errors.
- Snapshot detection is heuristic (name contains "snapshot" or "snap").
- ANSI color output is used; most modern terminals support it.

