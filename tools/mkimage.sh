#!/usr/bin/env bash
set -euo pipefail

KERNEL_BIN="${1:?kernel bin path}"
IMG="${2:?output img path}"

SIZE_MB="${SIZE_MB:-10}"

WORKDIR="$(mktemp -d)"
cleanup() {
  set +e
  sudo umount "$WORKDIR/mnt" 2>/dev/null || true
  sudo losetup -d "$LOOPDEV" 2>/dev/null || true
  rm -rf "$WORKDIR"
}
trap cleanup EXIT

mkdir -p "$WORKDIR/mnt"

# Create raw image
dd if=/dev/zero of="$IMG" bs=1M count="$SIZE_MB" status=none

# Partition: one primary partition starting at 1MiB
parted -s "$IMG" mklabel msdos
parted -s "$IMG" mkpart primary ext2 1MiB 100%
parted -s "$IMG" set 1 boot on

# Setup loop device with partitions
LOOPDEV=$(sudo losetup --find --show --partscan "$IMG")

# Format partition as ext2
sudo mkfs.ext2 -q "${LOOPDEV}p1"

# Mount
sudo mount "${LOOPDEV}p1" "$WORKDIR/mnt"

# Install files
sudo mkdir -p "$WORKDIR/mnt/boot/grub"
sudo cp "$KERNEL_BIN" "$WORKDIR/mnt/boot/kfs2.bin"
sudo cp "$(dirname "$0")/../grub/grub.cfg" "$WORKDIR/mnt/boot/grub/grub.cfg"

# Install GRUB to MBR of image (BIOS)
sudo grub-install --target=i386-pc --boot-directory="$WORKDIR/mnt/boot" --modules="normal multiboot ext2 part_msdos biosdisk" "$LOOPDEV" >/dev/null
sudo sync
echo "Built image: $IMG"
