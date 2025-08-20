#!/usr/bin/env sh
set -eu

PREFIX=${PREFIX:-/usr/local}
BINDIR=${BINDIR:-"$PREFIX/bin"}
SYSCONFDIR=${SYSCONFDIR:-/etc}

if [ ! -f "btrfs-monitor" ]; then
  echo "Building binary..."
  make
fi

echo "Installing to $BINDIR (requires root)"
install -d "$BINDIR"
install -m 0755 btrfs-monitor "$BINDIR/btrfs-monitor"

echo "Configuring default mount point..."
install -d "$SYSCONFDIR/btrfs-monitor"
printf "Enter default Btrfs mount point [/mnt/btrfs_test]: "
read MP || MP=""
MP=${MP:-/mnt/btrfs_test}
printf "mount_point=%s\n" "$MP" > "$SYSCONFDIR/btrfs-monitor/config"
echo "Saved to $SYSCONFDIR/btrfs-monitor/config"

echo "Done. Try: sudo btrfs-monitor"


