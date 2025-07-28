#ifndef BTRFS_H
#define BTRFS_H

int btrfs_path_exists(const char *path);
int is_btrfs(const char *path);
int has_compression(const char *mount_point);
int has_raid(const char *mount_point);

void list_subvolumes_and_snapshots(const char *mount_point);
void show_usage_plain(const char *path);

#endif
