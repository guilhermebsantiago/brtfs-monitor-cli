#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
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

static int ensure_directory_exists(const char *path) {
    if (path == NULL || path[0] == '\0') return -1;
    if (mkdir(path, 0755) == 0) return 0;
    if (errno == EEXIST) return 0;
    return -1;
}

static int write_config_file(const char *config_path, const char *mount_point) {
    if (!config_path || !mount_point) return -1;
    char dir[PATH_MAX];
    strncpy(dir, config_path, sizeof(dir) - 1);
    dir[sizeof(dir) - 1] = '\0';
    char *last_slash = strrchr(dir, '/');
    if (last_slash) {
        *last_slash = '\0';
        if (ensure_directory_exists(dir) != 0) {
            return -1;
        }
    }
    FILE *fp = fopen(config_path, "w");
    if (!fp) return -1;
    fprintf(fp, "mount_point=%s\n", mount_point);
    fclose(fp);
    return 0;
}

static const char *resolve_mount_point_with_first_run_prompt(int argc, char *argv[]) {
    const char *resolved = resolve_mount_point(argc, argv);
    if (resolved && strcmp(resolved, DEFAULT_BTRFS_PATH) != 0) {
        return resolved;
    }
    if (!isatty(STDIN_FILENO)) {
        return resolved;
    }
    char input[PATH_MAX];
    printf("Enter default Btrfs mount point [%s]: ", DEFAULT_BTRFS_PATH);
    fflush(stdout);
    if (!fgets(input, sizeof(input), stdin)) {
        return resolved;
    }
    size_t len = strlen(input);
    while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r' || input[len - 1] == ' ')) {
        input[len - 1] = '\0';
        len--;
    }
    const char *chosen = (len == 0) ? DEFAULT_BTRFS_PATH : input;

    char config_path[PATH_MAX];
    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    if (xdg_config_home && xdg_config_home[0] != '\0') {
        snprintf(config_path, sizeof(config_path), "%s/btrfs-monitor/config", xdg_config_home);
    } else {
        const char *home = getenv("HOME");
        if (!home || home[0] == '\0') {
            return chosen;
        }
        snprintf(config_path, sizeof(config_path), "%s/.config/btrfs-monitor/config", home);
    }
    if (write_config_file(config_path, chosen) == 0) {
        printf("Saved default mount point to %s\n", config_path);
    }
    return chosen;
}

int main(int argc, char *argv[]) {
    const char *mount_point = resolve_mount_point_with_first_run_prompt(argc, argv);

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
