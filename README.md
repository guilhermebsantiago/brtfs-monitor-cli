# Btrfs Monitor

Monitor simples em C para mostrar subvolumes, snapshots, uso de espaço e características do sistema de arquivos Btrfs.

---

## Objetivo

Este projeto foi desenvolvido para a matéria de Sistemas Operacionais e explora:

- Sistemas de arquivos modernos (Btrfs)
- Subvolumes e snapshots
- Compressão e RAID nativos
- Visualização de uso de disco com barras coloridas
- Integração com comandos do Linux e chamadas ao sistema (statvfs)

---

## Requisitos

Para rodar o projeto, você precisa de:

- Sistema operacional: Linux (Ubuntu no WSL2 funciona perfeitamente)
- Kernel Linux: com suporte a Btrfs (normalmente já vem por padrão no Ubuntu)
- Permissões: acesso ao volume Btrfs e comandos que podem exigir sudo
- Pacotes necessários:

```bash
sudo apt update  
sudo apt install gcc make btrfs-progs  
```
---

## Como usar

### 1. Criar ambiente Btrfs (opcional para testes)

Se quiser testar com um volume Btrfs real, execute:
```bash
fallocate -l 2G ~/btrfs-volume.img  
sudo mkfs.btrfs ~/btrfs-volume.img  
sudo mkdir -p /mnt/btrfs_test  
sudo mount ~/btrfs-volume.img /mnt/btrfs_test  

sudo btrfs subvolume create /mnt/btrfs_test/@root  
sudo btrfs subvolume create /mnt/btrfs_test/@home  
sudo btrfs subvolume create /mnt/btrfs_test/@var  

# Criar arquivos para simular uso
sudo fallocate -l 1G /mnt/btrfs_test/@home/data.img  
sudo fallocate -l 500M /mnt/btrfs_test/@var/log.img  

### 2. Ativar compressão em montagem (exemplo zstd)

Desmonte o volume e monte com compressão:

sudo umount /mnt/btrfs_test  
sudo mount -o compress=zstd ~/btrfs-volume.img /mnt/btrfs_test  
# Agora arquivos escritos aqui serão comprimidos automaticamente
```
### 3. Criar volume Btrfs com RAID (exemplo RAID1)

Simule dois discos com arquivos:

fallocate -l 1G ~/btrfs-disk1.img  
fallocate -l 1G ~/btrfs-disk2.img  

Formate em RAID1:

sudo mkfs.btrfs -d raid1 -m raid1 ~/btrfs-disk1.img ~/btrfs-disk2.img  

Monte:

sudo mkdir -p /mnt/btrfs_raid  
sudo mount ~/btrfs-disk1.img /mnt/btrfs_raid  

# (o mount pega o array RAID automaticamente)

---

### 4. Compilar o projeto

```bash
make  
```

### 5. Executar o monitor
```bash
./btrfs-monitor
```  

Você verá:

- Lista de subvolumes e snapshots com uso real em GB e barra colorida  
- Indicação se compressão está ativada no volume  
- Informação do tipo de RAID configurado (ex: raid0, raid1)  
- Uso total do volume   

---

## O que estamos usando do Sistema Operacional

- Kernel Linux: o projeto depende do suporte ao sistema de arquivos Btrfs, que está implementado no kernel Linux.  
- Comandos do Linux: o programa usa comandos externos (btrfs subvolume list, btrfs filesystem df, mount) para coletar dados.  
- Chamadas do sistema: usa statvfs() para obter dados reais de uso de disco.  
- Permissões: algumas operações requerem acesso elevado (sudo) para leitura e listagem dos subvolumes e snapshots.  

---

## Detalhes sobre Compressão e RAID

### Detectando compressão

O programa verifica se o volume está montado com compressão ativa lendo as opções de montagem ou o status do volume via `btrfs filesystem df` e exibe uma mensagem se estiver ativado.

---

### Detectando RAID

Btrfs suporta RAID nativo para dados e metadados. O monitor lê essa informação com `btrfs filesystem df` e exibe o tipo de RAID configurado (ex: single, raid0, raid1, raid10).

---

## Estrutura dos arquivos do projeto

- main.c: fluxo principal da aplicação — valida caminhos, detecta Btrfs, exibe informações.  
- btrfs.c / btrfs.h: funções para interagir com Btrfs — listar subvolumes, snapshots, checar compressão, RAID, calcular uso de espaço.  
- visual.c / visual.h: funções para exibir barras de uso coloridas no terminal.  
- Makefile: para compilar tudo facilmente com make.  
- .gitignore: para ignorar arquivos binários, objetos e temporários do editor.  

---

## Notas adicionais

- Execute o programa com permissão suficiente para acessar o ponto de montagem Btrfs.  
- Para usar snapshots, eles precisam existir na estrutura do seu volume — o programa detecta pela palavra "snapshot" no nome.  
- O uso das cores no terminal depende do suporte do seu terminal a ANSI escape codes (normalmente padrão).  

---

## Como limpar o projeto

```bash
make clean  
```

Remove os arquivos objeto e o binário compilado.  

---

## Contato e dúvidas

Este projeto foi desenvolvido para fins acadêmicos na matéria de Sistemas Operacionais. Para dúvidas ou sugestões, entre em contato com o autor.
