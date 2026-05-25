#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

check_pid() {
  local name="$1" file="${LOCAL_RUN}/${1}.pid"
  if [[ -f "$file" ]]; then
    local pid
    pid="$(cat "$file")"
    if kill -0 "$pid" 2>/dev/null; then
      printf '%s: running (pid %s)\n' "$name" "$pid"
      return 0
    fi
  fi
  printf '%s: stopped\n' "$name"
  return 1
}

printf 'MariaDB client: %s\n' "$(resolve_mysql_client)"
if mysql_exec_trinity -e "SELECT 1" &>/dev/null; then
  printf 'MariaDB: reachable as %s\n' "${MYSQL_USER:-trinity}"
else
  printf 'MariaDB: not reachable\n'
fi

check_pid bnetserver || true
check_pid worldserver || true

for port in 1119 8081 8085; do
  if wait_for_port 127.0.0.1 "$port" 1; then
    printf 'Port %s: open\n' "$port"
  else
    printf 'Port %s: closed\n' "$port"
  fi
done

realm_flag="$(mysql_exec_trinity auth -N -e "SELECT flag FROM realmlist WHERE id=1" 2>/dev/null || true)"
if [[ -n "$realm_flag" ]]; then
  if ((realm_flag & 2)); then
    printf 'Realm: OFFLINE (flag=%s)\n' "$realm_flag"
  else
    printf 'Realm: online (flag=%s)\n' "$realm_flag"
  fi
fi
