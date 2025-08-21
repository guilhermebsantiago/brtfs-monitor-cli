# btrfs-cli

Monitor simples em C para mostrar subvolumes, snapshots, uso de espaço e características do sistema de arquivos Btrfs.

---

## Objetivo

Este projeto foi desenvolvido para a matéria de Sistemas Operacionais e explora:

- Sistemas de arquivos modernos (Btrfs)
- Subvolumes e snapshots
- Compressão e RAID nativos
- Visualização de uso de disco com barras coloridas
- Integração com comandos do Linux e chamadas ao sistema (`statvfs`)

---

## Requisitos

- **Linux (Ubuntu no WSL2 funciona perfeitamente)**
- **Kernel com suporte ao Btrfs** (normalmente já vem por padrão no Ubuntu)
- **Pacotes obrigatórios**:

sudo apt update
sudo apt install gcc make btrfs-progs

---

## Como usar

### 1. Criar ambiente Btrfs (padrão)

fallocate -l 2G ~/btrfs-volume.img
sudo mkfs.btrfs ~/btrfs-volume.img
sudo mkdir -p /mnt/btrfs_test
sudo mount ~/btrfs-volume.img /mnt/btrfs_test

sudo btrfs subvolume create /mnt/btrfs_test/@root
sudo btrfs subvolume create /mnt/btrfs_test/@home
sudo btrfs subvolume create /mnt/btrfs_test/@var

# Criar arquivos de teste
sudo fallocate -l 1G /mnt/btrfs_test/@home/data.img
sudo fallocate -l 500M /mnt/btrfs_test/@var/log.img

---

### 2. Ativar compressão (zstd)

sudo umount /mnt/btrfs_test
sudo mount -o compress=zstd ~/btrfs-volume.img /mnt/btrfs_test

---

### 3. Criar volume Btrfs com RAID1 (opcional)

fallocate -l 1G ~/btrfs-disk1.img
fallocate -l 1G ~/btrfs-disk2.img

sudo mkfs.btrfs -d raid1 -m raid1 ~/btrfs-disk1.img ~/btrfs-disk2.img

# Associar os arquivos a /dev/loop
sudo losetup -fP ~/btrfs-disk1.img
sudo losetup -fP ~/btrfs-disk2.img

# Descobrir nomes (ex: /dev/loop0 e /dev/loop1)
losetup

# Escanear e montar
sudo btrfs device scan
sudo mkdir -p /mnt/btrfs_raid
sudo mount /dev/loop0 /mnt/btrfs_raid

---

### 4. Compilar o projeto

make

---

### 5. Executar o CLI

# Caminho padrão
sudo ./btrfs-cli

# Ou passando outro volume
sudo ./btrfs-cli /mnt/btrfs_raid

Você verá:

- Subvolumes e snapshots com uso real e barras coloridas
- Compressão ativada (se houver)
- RAID detectado (RAID0, RAID1, etc)
- Uso total do volume

---

## Detecção do sistema operacional

Este projeto utiliza:

- Chamadas ao **kernel Linux** para obter estatísticas (`statvfs`)
- Comandos de terminal (`btrfs`, `mount`) para extrair informações do sistema de arquivos
- Acesso a arquivos especiais de `/proc/mounts` e `/sys/fs/btrfs/`

---

## Compressão

Detectada automaticamente via:

- Opções de montagem (`compress=zstd`, `compress-force`, etc)
- Presença de blocos comprimidos no volume (`btrfs filesystem df`)

---

## RAID

Detectado via:

- `btrfs filesystem df <ponto_de_montagem>`
- Exibe se o volume está em modo RAID (RAID0, RAID1, RAID10, etc)

---

## Empacotamento (Debian/Launchpad)

Para gerar um pacote `.deb` localmente:

```
sudo apt install build-essential debhelper devscripts
debuild -us -uc
```

O binário será instalado como `btrfs-cli` em `/usr/bin/`.

---

## Estrutura dos arquivos

- `main.c`: fluxo principal
- `btrfs.c / btrfs.h`: funções para detectar RAID, compressão, subvolumes
- `visual.c / visual.h`: geração das barras coloridas
- `Makefile`: compila os arquivos com `make`
- `.gitignore`: ignora binários e temporários

---

## Como limpar o projeto

make clean

---

## Observações

- A execução requer `sudo` para acessar o volume e comandos de sistema
- Snapshots são detectados se o nome incluir `"snapshot"` (convencionais do `snapper`, por exemplo)
- Barras coloridas funcionam em terminais com suporte a ANSI (padrão no Ubuntu)

