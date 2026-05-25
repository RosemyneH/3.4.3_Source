#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

require_cmd git

TC_SQL_REF="${TC_SQL_REF:-master}"
TC_SQL_REPO="${TC_SQL_REPO:-https://github.com/TrinityCore/TrinityCore.git}"
CACHE_DIR="${LOCAL_CACHE}/trinitycore-sql"
SQL_LINK="${REPO_ROOT}/sql"

if [[ -d "${REPO_ROOT}/sql/Databases" ]]; then
  log_info "Using bundled sql/Databases/ (skip TrinityCore fetch)."
  exit 0
fi

ensure_local_dirs
if [[ ! -d "$CACHE_DIR/.git" ]]; then
  log_info "Sparse-cloning TrinityCore sql (${TC_SQL_REF})..."
  git clone --depth 1 --filter=blob:none --sparse "$TC_SQL_REPO" "$CACHE_DIR"
  git -C "$CACHE_DIR" sparse-checkout set sql
fi

git -C "$CACHE_DIR" fetch --depth 1 origin "$TC_SQL_REF" 2>/dev/null || true
git -C "$CACHE_DIR" checkout "$TC_SQL_REF" 2>/dev/null || git -C "$CACHE_DIR" checkout FETCH_HEAD 2>/dev/null || true

if [[ -L "$SQL_LINK" ]]; then
  rm -f "$SQL_LINK"
elif [[ -d "$SQL_LINK" && ! -d "${SQL_LINK}/base" ]]; then
  :
elif [[ -d "${SQL_LINK}/base" ]]; then
  log_info "sql/base already present."
  exit 0
fi

if [[ -d "${CACHE_DIR}/sql" ]]; then
  if [[ -e "$SQL_LINK" && ! -L "$SQL_LINK" ]]; then
    log_info "Merging sql/ from cache into ${SQL_LINK}..."
    cp -an "${CACHE_DIR}/sql/." "$SQL_LINK/"
  else
    rm -rf "$SQL_LINK"
    ln -sfn "${CACHE_DIR}/sql" "$SQL_LINK"
    log_info "Linked ${SQL_LINK} -> ${CACHE_DIR}/sql"
  fi
else
  log_err "No sql/ directory in ${CACHE_DIR}"
  exit 1
fi

log_info "Done."
