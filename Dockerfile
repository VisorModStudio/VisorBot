# syntax=docker/dockerfile:1

# ---------- Build Stage ----------
FROM debian:bookworm AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build git curl zip unzip tar pkg-config ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# vcpkg pinned to the exact same commit referenced by builtin-baseline in vcpkg.json,
# so build results here match CI/local builds. Full clone (no --depth) so `git show`
# on that commit always succeeds.
RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg \
    && git -C /vcpkg checkout ea1a7396b05637a53bf23c078647ecc0edee4b80 \
    && /vcpkg/bootstrap-vcpkg.sh -disableMetrics

WORKDIR /src
COPY . .

RUN cmake -B out -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DVCPKG_TARGET_TRIPLET=x64-linux \
      -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    && cmake --build out --config Release

# Fail the image build loudly if the binary still needs a shared vcpkg lib at runtime,
# instead of only discovering it later on the actual server.
RUN ldd out/VisorBot; \
    if ldd out/VisorBot | grep -qi "not found"; then \
      echo "ERROR: VisorBot has unresolved shared library dependencies"; exit 1; \
    fi

# ---------- Runtime Stage ----------
FROM debian:bookworm-slim AS runtime

# Only what the compiled binary + libs (openssl, ca certs for Discord/HTTPS) actually need.
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Binary lives OUTSIDE /home/container so Pterodactyl's persistent volume mount
# never shadows it.
COPY --from=build /src/out/VisorBot /usr/local/bin/VisorBot
RUN chmod +x /usr/local/bin/VisorBot

# /home/container is where Pterodactyl mounts the server's persistent storage
# (config files, the SQLite DB, etc.) — the bot should read/write here.
WORKDIR /home/container

# Wings runs the egg's configured startup command inside the container itself,
# so this CMD is only relevant when you run the image standalone (e.g. for local testing).
CMD ["/usr/local/bin/VisorBot"]
