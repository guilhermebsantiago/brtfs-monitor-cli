#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btrfs.h"
#include "visual.h"

#define BTRFS_PATH "/mnt/btrfs_test"

int main() {
    printf("📂 Lendo sistema de arquivos em: %s\n\n", BTRFS_PATH);

    if (!btrfs_path_exists(BTRFS_PATH)) {
        fprintf(stderr, "⚠️  O caminho %s não existe ou não está montado.\n", BTRFS_PATH);
        return 1;
    }

    if (!is_btrfs(BTRFS_PATH)) {
        printf("⚠️  %s não é um sistema Btrfs. Continuando sem funcionalidades Btrfs...\n\n", BTRFS_PATH);
        show_usage_plain(BTRFS_PATH);
        return 0;
    }

    printf("✅ Sistema Btrfs detectado!\n\n");

    if (has_compression(BTRFS_PATH)) {
        printf("✨ Compressão ativada neste volume.\n");
    } else {
        printf("⚠️ Compressão não detectada.\n");
    }

    if (has_raid(BTRFS_PATH)) {
        printf("🛡️ RAID ativo neste volume.\n");
    } else {
        printf("⚠️ RAID não detectado.\n");
    }
    printf("\n");

    // Listar subvolumes e snapshots
    list_subvolumes_and_snapshots(BTRFS_PATH);

    // Mostrar uso do volume geral
    show_usage_plain(BTRFS_PATH);

    return 0;
}
