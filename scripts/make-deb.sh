#!/usr/bin/env bash
set -euo pipefail

APP=btrfs-monitor
VERSION=${VERSION:-0.1.0}
PKGDIR=dist/${APP}_${VERSION}

rm -rf dist
mkdir -p "$PKGDIR/DEBIAN" "$PKGDIR/usr/local/bin" "$PKGDIR/etc/btrfs-monitor"

cat > "$PKGDIR/DEBIAN/control" <<EOF
Package: $APP
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: $(whoami)
Description: Btrfs monitor CLI showing subvolumes, snapshots, and usage
EOF

make clean && make
install -m 0755 $APP "$PKGDIR/usr/local/bin/$APP"
echo "mount_point=/mnt/btrfs_test" > "$PKGDIR/etc/btrfs-monitor/config"

dpkg-deb --build "$PKGDIR"
echo "DEB built at dist/${APP}_${VERSION}.deb"


