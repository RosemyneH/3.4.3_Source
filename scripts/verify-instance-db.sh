#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

require_cmd "$(basename "$(resolve_mysql_client)")"

MYSQL_USER="${MYSQL_USER:-trinity}"
MYSQL_PASS="${MYSQL_PASS:-trinity}"

log_info "Checking characters DB instance lock schema..."
verify_instance_schema() {
  local db="$1"
  local missing
  missing=$(mysql_exec "${MYSQL_USER}" "${MYSQL_PASS}" "$db" -N -e "
    SELECT column_name FROM (
      SELECT 'character_instance_lock.difficulty' AS column_name
      UNION SELECT 'character_instance_lock.entranceWorldSafeLocId'
      UNION SELECT 'instance.entranceWorldSafeLocId'
    ) expected
    WHERE column_name NOT IN (
      SELECT CONCAT(table_name, '.', column_name)
      FROM information_schema.columns
      WHERE table_schema = DATABASE()
        AND table_name IN ('character_instance_lock', 'instance')
    );
  " 2>/dev/null || true)
  if [[ -n "$missing" ]]; then
    log_err "Characters DB missing instance lock columns:"
    echo "$missing"
    return 1
  fi
  for table in character_instance_lock instance account_instance_times; do
    if ! mysql_exec "${MYSQL_USER}" "${MYSQL_PASS}" "$db" -N -e \
      "SELECT 1 FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name = '${table}'" 2>/dev/null | grep -q 1; then
      log_err "Characters DB missing table: ${table}"
      return 1
    fi
  done
  log_info "Instance lock schema OK (${db})."
}

verify_instance_schema characters
log_info "Done."
