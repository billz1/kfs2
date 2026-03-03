FROM debian:bookworm-slim

# Toolchain + ISO tooling for a 32-bit freestanding kernel (KFS2)
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-multilib \
    nasm \
    grub-pc-bin \
    grub-common \
    xorriso \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# Copy sources (the repo is typically bind-mounted in docker-compose; this
# keeps `docker build` usable too).
COPY . /workspace

# Default: build kernel + GRUB ISO
CMD ["bash", "-lc", "make fclean && make all && make iso"]
