#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

SKIP_DEPS=0 SKIP_BUILD=0 SKIP_SQL=0 SKIP_DB=0 SKIP_EXTRACT=0 SKIP_START=0
DB_IMPORT=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-deps) SKIP_DEPS=1; shift ;;
    --skip-build) SKIP_BUILD=1; shift ;;
    --skip-sql) SKIP_SQL=1; shift ;;
    --skip-db) SKIP_DB=1; shift ;;
    --skip-extract) SKIP_EXTRACT=1; shift ;;
    --skip-start) SKIP_START=1; shift ;;
    --import-db) DB_IMPORT=1; shift ;;
    -h|--help)
      cat <<EOF
Usage: $0 [options]
  --skip-deps      Skip install-deps.sh
  --skip-build     Skip build.sh
  --skip-sql       Skip fetch-sql.sh / fetch-tdb.sh
  --skip-db        Skip setup-database.sh
  --skip-extract   Skip extract-client.sh
  --skip-start     Skip start.sh
  --import-db      Pass --import to setup-database.sh
EOF
      exit 0
      ;;
    *) log_err "Unknown option: $1"; exit 1 ;;
  esac
done

[[ -f "$LOCAL_ENV" ]] || log_warn "No local.env — copy local.env.example to local.env"
load_local_env

[[ "$SKIP_DEPS" -eq 0 ]] && "${SCRIPT_DIR}/install-deps.sh"
[[ "$SKIP_BUILD" -eq 0 ]] && "${SCRIPT_DIR}/build.sh"
[[ "$SKIP_SQL" -eq 0 ]] && { "${SCRIPT_DIR}/fetch-sql.sh" || true; "${SCRIPT_DIR}/fetch-tdb.sh" || true; }
"${SCRIPT_DIR}/setup-config.sh"
if [[ "$SKIP_DB" -eq 0 ]]; then
  if [[ "$DB_IMPORT" -eq 1 ]]; then
    "${SCRIPT_DIR}/setup-database.sh" --import
  else
    "${SCRIPT_DIR}/setup-database.sh"
  fi
fi
if [[ "$SKIP_EXTRACT" -eq 0 && -n "${WOW_CLIENT_PATH:-}" ]]; then
  "${SCRIPT_DIR}/extract-client.sh" --skip-mmaps
elif [[ "$SKIP_EXTRACT" -eq 0 ]]; then
  log_info "WOW_CLIENT_PATH unset — skipping extraction."
fi
[[ "$SKIP_START" -eq 0 ]] && "${SCRIPT_DIR}/start.sh"

log_info "Deploy complete. Run ./scripts/status.sh to verify."
