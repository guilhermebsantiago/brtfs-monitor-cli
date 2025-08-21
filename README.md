# btrfs-cli

Simple C CLI to display Btrfs subvolumes, snapshots, space usage, and filesystem features.

---

## Goal

This project was built for an Operating Systems course and explores:

- Modern filesystems (Btrfs)
- Subvolumes and snapshots
- Native compression and RAID
- Disk usage visualization with color bars
- Integration with Linux commands and system calls (`statvfs`)

---

## Requirements

- **Linux (Ubuntu on WSL2 works great)**
- **Kernel with Btrfs support** (Ubuntu includes it by default)
- **Required packages**:

```bash
sudo apt update
sudo apt install gcc make btrfs-progs
```

Useful docs: `btrfs-progs` [GitHub](https://github.com/kdave/btrfs-progs), Btrfs [Wiki](https://btrfs.wiki.kernel.org/), Btrfs [Docs](https://btrfs.readthedocs.io/).

---

## How to use

### 1) Create a Btrfs environment (single device)

```bash
fallocate -l 2G ~/btrfs-volume.img
sudo mkfs.btrfs ~/btrfs-volume.img
sudo mkdir -p /mnt/btrfs_test
sudo mount ~/btrfs-volume.img /mnt/btrfs_test

# Create some subvolumes
sudo btrfs subvolume create /mnt/btrfs_test/@root
sudo btrfs subvolume create /mnt/btrfs_test/@home
sudo btrfs subvolume create /mnt/btrfs_test/@var

# Create test files
sudo fallocate -l 1G /mnt/btrfs_test/@home/data.img
sudo fallocate -l 500M /mnt/btrfs_test/@var/log.img
```

References: Subvolumes and snapshots [docs](https://btrfs.readthedocs.io/en/latest/Subvolumes.html).

---

### 2) Enable compression (zstd)

```bash
sudo umount /mnt/btrfs_test
sudo mount -o compress=zstd ~/btrfs-volume.img /mnt/btrfs_test
```

More on compression: Btrfs compression [guide](https://btrfs.readthedocs.io/en/latest/Compression.html).

---

### 3) Create a Btrfs RAID1 filesystem (optional)

```bash
fallocate -l 1G ~/btrfs-disk1.img
fallocate -l 1G ~/btrfs-disk2.img

sudo mkfs.btrfs -d raid1 -m raid1 ~/btrfs-disk1.img ~/btrfs-disk2.img

# Attach the image files to loop devices
sudo losetup -fP ~/btrfs-disk1.img
sudo losetup -fP ~/btrfs-disk2.img

# Identify loop names (e.g., /dev/loop0 and /dev/loop1)
losetup

# Scan and mount
sudo btrfs device scan
sudo mkdir -p /mnt/btrfs_raid
sudo mount /dev/loop0 /mnt/btrfs_raid
```

More on RAID profiles: Btrfs RAID [profiles](https://btrfs.readthedocs.io/en/latest/Administration.html#raid-profiles).

---

### 4) Build the project

```bash
make
```

---

### 5) Run the CLI

```bash
# Default mount point
sudo ./btrfs-cli

# Or pointing to another mount
sudo ./btrfs-cli /mnt/btrfs_raid
```

You will see:

- Subvolumes and snapshots (with realistic usage and color bars)
- Compression status (if enabled)
- RAID profile detection (RAID0, RAID1, etc.)
- Overall filesystem usage

---

## How the OS/filesystem info is detected

This project uses:

- Linux system calls for stats (`statvfs`) — see man page: [`statvfs(3)`](https://man7.org/linux/man-pages/man3/statvfs.3.html)
- CLI tools (`btrfs`, `mount`) to extract filesystem info — see [`btrfs` command overview](https://btrfs.readthedocs.io/en/latest/btrfs.html)
- Special files in `/proc/mounts` and `/sys/fs/btrfs/`

---

## Compression

Automatically detected via:

- Mount options (e.g., `compress=zstd`, `compress-force`)
- Presence of compressed data/metadata in `btrfs filesystem df`

Links: `btrfs filesystem df` [reference](https://btrfs.readthedocs.io/en/latest/btrfs-filesystem.html#df), Compression [guide](https://btrfs.readthedocs.io/en/latest/Compression.html).

---

## RAID

Detected using:

- `btrfs filesystem df <mountpoint>` (shows RAID profile for data/metadata/system)

Links: RAID profiles [docs](https://btrfs.readthedocs.io/en/latest/Administration.html#raid-profiles).

---

## Packaging (Debian/Launchpad)

Build a local `.deb` package:

```bash
sudo apt install build-essential debhelper devscripts
debuild -us -uc
```

The binary installs as `btrfs-cli` in `/usr/bin/`.

Docs: `debhelper` [guide](https://www.debian.org/doc/manuals/maint-guide/), `devscripts` [package](https://tracker.debian.org/pkg/devscripts).

---

## Project structure

- `main.c`: main program flow
- `btrfs.c` / `btrfs.h`: functions to detect RAID, compression, subvolumes
- `visual.c` / `visual.h`: color bar rendering
- `Makefile`: builds with `make`
- `.gitignore`: ignores binaries and temporary files

---

## Cleaning the project

```bash
make clean
```

---

## Notes

- Running requires `sudo` to access mounts and system commands
- Snapshots are detected if the name contains `"snapshot"` (common with `snapper`)
- Color bars require an ANSI-capable terminal (default in Ubuntu)

---

## Useful links

- Btrfs Wiki: https://btrfs.wiki.kernel.org/
- Btrfs Docs: https://btrfs.readthedocs.io/
- btrfs-progs (userspace tools): https://github.com/kdave/btrfs-progs
- `btrfs` command overview: https://btrfs.readthedocs.io/en/latest/btrfs.html
- `btrfs filesystem` subcommands: https://btrfs.readthedocs.io/en/latest/btrfs-filesystem.html
- Compression: https://btrfs.readthedocs.io/en/latest/Compression.html
- RAID profiles: https://btrfs.readthedocs.io/en/latest/Administration.html#raid-profiles
- statvfs(3) man page: https://man7.org/linux/man-pages/man3/statvfs.3.html