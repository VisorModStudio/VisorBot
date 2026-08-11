FROM debian:bookworm AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build git curl zip unzip tar pkg-config ca-certificates \
    && rm -rf /var/lib/apt/lists/*


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


RUN mkdir -p /collected-libs && \
    find /src/out/vcpkg_installed/x64-linux/lib -maxdepth 1 -name '*.so*' \
      -exec cp -av {} /collected-libs/ \; ; \
    echo "Collected libs:" && ls -la /collected-libs


FROM debian:bookworm-slim AS runtime


RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates libssl3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /src/out/VisorBot /usr/local/bin/VisorBot
COPY --from=build /collected-libs/ /usr/local/lib/
RUN chmod +x /usr/local/bin/VisorBot \
    && ldconfig

ENV LD_LIBRARY_PATH=/usr/local/lib


RUN ldd /usr/local/bin/VisorBot; \
    if ldd /usr/local/bin/VisorBot | grep -qi "not found"; then \
      echo "ERROR: VisorBot has unresolved shared library dependencies"; exit 1; \
    fi


WORKDIR /home/container


CMD ["/usr/local/bin/VisorBot"]