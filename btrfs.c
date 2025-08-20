#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include "btrfs.h"
#include "visual.h"
#include <sys/stat.h>
#include <sys/vfs.h>
#define BTRFS_MAGIC 0x9123683E


#define MAX_LINE 1024

int btrfs_path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

int is_btrfs(const char *path) {
    struct statfs stfs;
    if (statfs(path, &stfs) != 0) {
        perror("statfs");
        return 0;
    }
    return (stfs.f_type == BTRFS_MAGIC);
}

int has_compression(const char *mount_point) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "mount | grep ' %s ' | grep -oE 'compress=[^ ,]+'", mount_point);
    FILE *fp = popen(cmd, "r");
    if (!fp) return 0;

    char line[128];
    int found = 0;
    if (fgets(line, sizeof(line), fp)) {
        found = 1;
    }
    pclose(fp);
    return found;
}

int has_raid(const char *mount_point) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "btrfs filesystem df %s 2>/dev/null", mount_point);
    FILE *fp = popen(cmd, "r");
    if (!fp) return 0;

    char line[512];
    int raid_detected = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "RAID")) {
            raid_detected = 1;
            break;
        }
    }
    pclose(fp);
    return raid_detected;
}

void print_line_trimmed(char *line) {
    int len = (int)strlen(line);
    while (len > 0 && (line[len-1] == '\n' || line[len-1] == ' ' || line[len-1] == '\r')) {
        line[len-1] = '\0';
        len--;
    }
}

void list_subvolumes_and_snapshots(const char *mount_point) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "sudo btrfs subvolume list -o %s", mount_point);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("Failed to list subvolumes");
        return;
    }

    printf("📦 Subvolumes and snapshots:\n");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        print_line_trimmed(line);

        char *p = strstr(line, "path ");
        if (!p) continue;
        p += 5;

        int is_snapshot = 0;
        if (strstr(p, "snapshot") || strstr(p, "snap")) {
            is_snapshot = 1;
        }

        long total_gb = 0, used_gb = 0;
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", mount_point, p);
        struct statvfs sfs;
        if (statvfs(full_path, &sfs) == 0) {
            unsigned long block_size = sfs.f_frsize;
            unsigned long total_blocks = sfs.f_blocks;
            unsigned long free_blocks = sfs.f_bfree;
            unsigned long used_blocks = total_blocks - free_blocks;

            total_gb = (total_blocks * block_size) / (1024 * 1024 * 1024);
            used_gb = (used_blocks * block_size) / (1024 * 1024 * 1024);
            if (total_gb == 0) total_gb = 1; 
        } else {
            total_gb = 1;
            used_gb = 0;
        }

        int percent = (int)((used_gb * 100) / total_gb);

        if (is_snapshot) {
            printf("📸Snapshot: %s - Used: %ld GB of %ld GB\n", p, used_gb, total_gb);
            print_usage_bar_colored(percent, COLOR_YELLOW);
        } else {
            printf("Subvolume: %s - Used: %ld GB of %ld GB\n", p, used_gb, total_gb);
            print_usage_bar_colored(percent, COLOR_GREEN);
        }
    }
    printf("\n");
    pclose(fp);
}

void show_usage_plain(const char *path) {
    struct statvfs sfs;
    if (statvfs(path, &sfs) != 0) {
        perror("Failed to read filesystem statistics");
        return;
    }

    unsigned long block_size = sfs.f_frsize;
    unsigned long total_blocks = sfs.f_blocks;
    unsigned long free_blocks = sfs.f_bfree;
    unsigned long used_blocks = total_blocks - free_blocks;

    long total_gb = (total_blocks * block_size) / (1024 * 1024 * 1024);
    long used_gb = (used_blocks * block_size) / (1024 * 1024 * 1024);
    if (total_gb == 0) total_gb = 1;

    int percent = (int)((used_gb * 100) / total_gb);

    printf("📊 Total usage of %s: %ld GB of %ld GB\n", path, used_gb, total_gb);
    print_usage_bar_colored(percent, COLOR_BLUE);
}
