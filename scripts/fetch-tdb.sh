#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

require_cmd curl

TDB_WORLD_FILE="${TDB_WORLD_FILE:-TDB_full_world_343.23121_2023_12_20.sql}"
TDB_HOTFIXES_FILE="${TDB_HOTFIXES_FILE:-TDB_full_hotfixes_343.23121_2023_12_20.sql}"
TDB_RELEASE_URL="${TDB_RELEASE_URL:-https://github.com/TrinityCore/TrinityCore/releases/latest/download}"

ensure_local_dirs

if [[ -f "${REPO_ROOT}/sql/Databases/world.sql" ]]; then
  log_info "Using bundled sql/Databases/world.sql (skip TDB download)."
  ln -sf "${REPO_ROOT}/sql/Databases/world.sql" "${LOCAL_SQL}/${TDB_WORLD_FILE}" 2>/dev/null || \
    cp -n "${REPO_ROOT}/sql/Databases/world.sql" "${LOCAL_SQL}/${TDB_WORLD_FILE}" 2>/dev/null || true
  ln -sf "${REPO_ROOT}/sql/Databases/hotfixes.sql" "${LOCAL_SQL}/${TDB_HOTFIXES_FILE}" 2>/dev/null || \
    cp -n "${REPO_ROOT}/sql/Databases/hotfixes.sql" "${LOCAL_SQL}/${TDB_HOTFIXES_FILE}" 2>/dev/null || true
fi

fetch_one() {
  local name="$1"
  local dest="${LOCAL_SQL}/${name}"
  if [[ -f "$dest" ]]; then
    log_info "Already have ${name}"
    return 0
  fi
  log_info "Downloading ${name}..."
  curl -fL --retry 3 -o "$dest" "${TDB_RELEASE_URL}/${name}" || {
    log_warn "Download failed for ${name}. Place file in ${LOCAL_SQL}/ or use sql/Databases/*.sql"
    return 1
  }
}

fetch_one "$TDB_WORLD_FILE" || true
fetch_one "$TDB_HOTFIXES_FILE" || true

mkdir -p "$BIN_DIR"
for f in "$TDB_WORLD_FILE" "$TDB_HOTFIXES_FILE"; do
  if [[ -f "${LOCAL_SQL}/${f}" ]]; then
    ln -sf "${LOCAL_SQL}/${f}" "${BIN_DIR}/${f}" 2>/dev/null || cp -n "${LOCAL_SQL}/${f}" "${BIN_DIR}/${f}"
  fi
done

log_info "TDB files ready under ${LOCAL_SQL}/ and ${BIN_DIR}/"
