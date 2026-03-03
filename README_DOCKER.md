# Docker setup (KFS2)

This repo can be built in a reproducible way using Docker.

## Requirements

- Docker Desktop / Docker Engine
- docker compose (usually included)

## Build kernel + ISO

From the project root:

```bash
docker compose up --build kfs2
```

Artifacts will be created in `./build/` on your host:

- `build/kfs2.bin`
- `build/kfs2.iso`

## Run with QEMU (recommended)

Install QEMU on your host and run:

```bash
qemu-system-i386 -boot d -cdrom build/kfs2.iso -m 256M
```

## (Optional) Run QEMU inside Docker

Linux hosts only (needs `--privileged`/KVM access). This is provided as a convenience:

```bash
docker compose up --build run-iso
```

## Clean

```bash
docker compose run --rm kfs2 bash -lc "make fclean"
```
