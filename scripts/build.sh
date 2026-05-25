#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

require_cmd cmake

BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
CMAKE_ARGS=(
  -S "$REPO_ROOT"
  -B "${REPO_ROOT}/${BUILD_DIR}"
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
  -DTOOLS=ON
  -DSERVERS=ON
)

if is_arch && command -v mariadb_config &>/dev/null; then
  CMAKE_ARGS+=(-DMYSQL_EXECUTABLE="$(command -v mariadb)")
fi
if is_arch; then
  CMAKE_ARGS+=(-DNOJEM=ON)
  "${SCRIPT_DIR}/install-boost-process.sh"
fi

log_info "Configuring (${BUILD_TYPE})..."
cmake "${CMAKE_ARGS[@]}"

log_info "Building..."
cmake --build "${REPO_ROOT}/${BUILD_DIR}" -j"$(nproc)"

ensure_local_dirs
ensure_build_bins
for conf in worldserver.conf bnetserver.conf; do
  dist="${BIN_DIR}/${conf}.dist"
  if [[ -f "$dist" && ! -e "${BIN_DIR}/${conf}" ]]; then
    ln -sf "${conf}.dist" "${BIN_DIR}/${conf}" 2>/dev/null || cp "$dist" "${BIN_DIR}/${conf}"
  fi
done

if [[ -f "${REPO_ROOT}/src/server/bnetserver/bnetserver.cert.pem" ]]; then
  cp -f "${REPO_ROOT}/src/server/bnetserver/bnetserver.cert.pem" "${BIN_DIR}/" 2>/dev/null || true
  cp -f "${REPO_ROOT}/src/server/bnetserver/bnetserver.key.pem" "${BIN_DIR}/" 2>/dev/null || true
fi

log_info "Binaries in ${BIN_DIR}"
log_info "Done. Next: ./scripts/setup-config.sh"
