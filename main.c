#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "btrfs.h"
#include "visual.h"

#define DEFAULT_BTRFS_PATH "/mnt/btrfs"

static int parse_config_file_for_mount_point(const char *config_path, char *out_buffer, size_t out_buffer_size) {
    FILE *fp = fopen(config_path, "r");
    if (!fp) return 0;

    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        const char *key1 = "mount_point=";
        const char *key2 = "mountpoint=";
        if (strncmp(line, key1, (int)strlen(key1)) == 0) {
            strncpy(out_buffer, line + strlen(key1), out_buffer_size - 1);
            found = 1;
        } else if (strncmp(line, key2, (int)strlen(key2)) == 0) {
            strncpy(out_buffer, line + strlen(key2), out_buffer_size - 1);
            found = 1;
        }
        if (found) {
            size_t len = strlen(out_buffer);
            while (len > 0 && (out_buffer[len - 1] == '\n' || out_buffer[len - 1] == '\r' || out_buffer[len - 1] == ' ')) {
                out_buffer[len - 1] = '\0';
                len--;
            }
            break;
        }
    }
    fclose(fp);
    return found;
}

static const char *resolve_mount_point(int argc, char *argv[]) {
    if (argc > 1 && argv[1] && argv[1][0] != '\0') {
        return argv[1];
    }

    const char *env_mp = getenv("BTRFS_MONITOR_MOUNTPOINT");
    if (env_mp && env_mp[0] != '\0') {
        return env_mp;
    }

    static char config_mount_point[PATH_MAX];

    // Try config files in order
    if (parse_config_file_for_mount_point("/etc/btrfs-monitor/config", config_mount_point, sizeof(config_mount_point))) {
        return config_mount_point;
    }

    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    if (xdg_config_home && xdg_config_home[0] != '\0') {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/btrfs-monitor/config", xdg_config_home);
        if (parse_config_file_for_mount_point(path, config_mount_point, sizeof(config_mount_point))) {
            return config_mount_point;
        }
    }

    const char *home = getenv("HOME");
    if (home && home[0] != '\0') {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/.config/btrfs-monitor/config", home);
        if (parse_config_file_for_mount_point(path, config_mount_point, sizeof(config_mount_point))) {
            return config_mount_point;
        }
    }

    return DEFAULT_BTRFS_PATH;
}

int main(int argc, char *argv[]) {
    const char *mount_point = resolve_mount_point(argc, argv);

    printf("Reading filesystem at: %s\n\n", mount_point);

    if (!btrfs_path_exists(mount_point)) {
        fprintf(stderr, "Warning: path %s does not exist or is not mounted.\n", mount_point);
        return 1;
    }

    if (!is_btrfs(mount_point)) {
        printf("Warning: %s is not a Btrfs filesystem. Continuing without Btrfs-specific features...\n\n", mount_point);
        show_usage_plain(mount_point);
        return 0;
    }

    printf("Btrfs filesystem detected.\n\n");

    if (has_compression(mount_point)) {
        printf("Compression is enabled on this volume.\n");
    } else {
        printf("Compression not detected.\n");
    }

    if (has_raid(mount_point)) {
        printf("RAID is active on this volume.\n");
    } else {
        printf("RAID not detected.\n");
    }
    printf("\n");

    // List subvolumes and snapshots
    list_subvolumes_and_snapshots(mount_point);

    // Show overall usage
    show_usage_plain(mount_point);

    return 0;
}
