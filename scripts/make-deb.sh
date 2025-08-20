#!/usr/bin/env bash
set -euo pipefail

APP=btrfs-monitor
VERSION=${VERSION:-0.1.0}

# Build in a Linux filesystem (e.g., /tmp) to avoid NTFS permission issues on WSL
STAGE_ROOT=$(mktemp -d)
PKGROOT="$STAGE_ROOT/${APP}_${VERSION}"

rm -rf dist
mkdir -p dist "$PKGROOT/DEBIAN" "$PKGROOT/usr/local/bin" "$PKGROOT/etc/btrfs-monitor"

cat > "$PKGROOT/DEBIAN/control" <<EOF
Package: $APP
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: $(whoami)
Description: Btrfs monitor CLI showing subvolumes, snapshots, and usage
EOF

make clean && make
install -m 0755 "$APP" "$PKGROOT/usr/local/bin/$APP"
echo "mount_point=/mnt/btrfs_test" > "$PKGROOT/etc/btrfs-monitor/config"

# Ensure correct permissions for Debian policy
chmod 0755 "$PKGROOT/DEBIAN"
chmod 0644 "$PKGROOT/DEBIAN/control"
chmod 0755 "$PKGROOT/usr" "$PKGROOT/usr/local" "$PKGROOT/usr/local/bin" "$PKGROOT/etc" "$PKGROOT/etc/btrfs-monitor"
chmod 0644 "$PKGROOT/etc/btrfs-monitor/config"

# Build the .deb and place it in dist/
dpkg-deb --build --root-owner-group "$PKGROOT" "dist/${APP}_${VERSION}.deb"

# Clean staging area
rm -rf "$STAGE_ROOT"

echo "DEB built at dist/${APP}_${VERSION}.deb"


