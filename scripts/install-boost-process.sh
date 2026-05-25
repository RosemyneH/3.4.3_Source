#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "${SCRIPT_DIR}/lib/common.sh"

BOOST_PROCESS_REF="${BOOST_PROCESS_REF:-boost-1.86.0}"
DEST="${LOCAL_CACHE}/boost-process/include/boost/process"
CACHE_REPO="${LOCAL_CACHE}/boost-process-src"

if [[ -f "${DEST}/args.hpp" ]]; then
  log_info "Boost.Process headers already installed."
  exit 0
fi

require_cmd git
ensure_local_dirs

if [[ ! -d "$CACHE_REPO/.git" ]]; then
  log_info "Fetching Boost.Process (${BOOST_PROCESS_REF})..."
  git clone --depth 1 --branch "$BOOST_PROCESS_REF" \
    https://github.com/boostorg/process.git "$CACHE_REPO"
fi

rm -rf "${LOCAL_CACHE}/boost-process/include"
mkdir -p "${LOCAL_CACHE}/boost-process/include/boost"
cp -a "${CACHE_REPO}/include/boost/process" "${LOCAL_CACHE}/boost-process/include/boost/"
log_info "Boost.Process headers at ${LOCAL_CACHE}/boost-process/include"
