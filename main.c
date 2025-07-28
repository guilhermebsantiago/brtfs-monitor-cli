#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btrfs.h"
#include "visual.h"

#define DEFAULT_BTRFS_PATH "/mnt/btrfs_test"

int main(int argc, char *argv[]) {
    const char *mount_point = (argc > 1) ? argv[1] : DEFAULT_BTRFS_PATH;

    printf("📂 Lendo sistema de arquivos em: %s\n\n", mount_point);

    if (!btrfs_path_exists(mount_point)) {
        fprintf(stderr, "⚠️  O caminho %s não existe ou não está montado.\n", mount_point);
        return 1;
    }

    if (!is_btrfs(mount_point)) {
        printf("⚠️  %s não é um sistema Btrfs. Continuando sem funcionalidades Btrfs...\n\n", mount_point);
        show_usage_plain(mount_point);
        return 0;
    }

    printf("✅ Sistema Btrfs detectado!\n\n");

    if (has_compression(mount_point)) {
        printf("✨ Compressão ativada neste volume.\n");
    } else {
        printf("⚠️ Compressão não detectada.\n");
    }

    if (has_raid(mount_point)) {
        printf("🛡️ RAID ativo neste volume.\n");
    } else {
        printf("⚠️ RAID não detectado.\n");
    }
    printf("\n");

    // Listar subvolumes e snapshots
    list_subvolumes_and_snapshots(mount_point);

    // Mostrar uso do volume geral
    show_usage_plain(mount_point);

    return 0;
}
