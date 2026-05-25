#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

WOW_CLIENT_PATH="${WOW_CLIENT_PATH:-}"
if [[ -z "$WOW_CLIENT_PATH" || ! -d "$WOW_CLIENT_PATH" ]]; then
  log_err "Set WOW_CLIENT_PATH in local.env (install root with Data/)."
  exit 1
fi

resolve_config_wtf() {
  local base="$1" candidates=()
  candidates=(
    "${base}/_classic_/WTF/Config.wtf"
    "${base}/WTF/Config.wtf"
  )
  local path
  for path in "${candidates[@]}"; do
    [[ -f "$path" ]] && { printf '%s\n' "$path"; return 0; }
  done
  return 1
}

CONFIG_WTF="$(resolve_config_wtf "$WOW_CLIENT_PATH")" || {
  log_err "No Config.wtf under ${WOW_CLIENT_PATH} (_classic_/WTF or WTF)."
  exit 1
}

patch_wtf() {
  local key="$1" value="$2" file="$3"
  if grep -q "^SET ${key} " "$file" 2>/dev/null; then
    sed -i "s|^SET ${key} .*|SET ${key} \"${value}\"|" "$file"
  else
    printf 'SET %s "%s"\n' "$key" "$value" >>"$file"
  fi
}

REALM_HOST="${REALM_HOST:-127.0.0.1}"
BACKUP="${CONFIG_WTF}.bak.$(date +%Y%m%d%H%M%S)"
cp -a "$CONFIG_WTF" "$BACKUP"
log_info "Backed up Config.wtf -> $(basename "$BACKUP")"

patch_wtf portal "$REALM_HOST" "$CONFIG_WTF"
patch_wtf useIPv6 "0" "$CONFIG_WTF"
patch_wtf agentUID "${CASC_PRODUCT:-wow_classic}" "$CONFIG_WTF"

install_aio_addon() {
  local src="${REPO_ROOT}/client-addons/AIO"
  local dst="${WOW_CLIENT_PATH}/_classic_/Interface/AddOns/AIO"
  if [[ ! -d "$src" ]]; then
    log_warn "AIO addon source missing at ${src}"
    return 0
  fi
  mkdir -p "$(dirname "$dst")"
  rm -rf "$dst"
  cp -a "$src" "$dst"
  log_info "Installed AIO addon -> _classic_/Interface/AddOns/AIO"
}

install_aio_addon

log_info "Client Config.wtf: portal=${REALM_HOST}, useIPv6=0"
log_info "Launch from: ${WOW_CLIENT_PATH}/_classic_/WowClassic.exe (or your launcher)"
log_info "Realm in DB: ${REALM_NAME:-Trinity} @ ${REALM_HOST}:${REALM_PORT:-8085} (build ${REALM_GAMEBUILD:-54261})"
log_info "In-game: /aiodev opens the AIO localhost dev panel"
