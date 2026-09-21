#!/bin/bash
# Flutter SDK installer for the Feather-Krita automation box.
# (Recreated in loop-36 — box reset #2 wiped /home/z/flutter.)
# Version 3.35.3 = the CI arbitration pin (build-app.yml), so local
# analyze results match CI exactly (no 3.47-vs-3.35 drift).
set -e
V=3.35.3
TARBALL=/home/z/flutter_install/flutter_linux_${V}-stable.tar.xz
mkdir -p /home/z/flutter_install
# ALWAYS run the download step: -C - resumes a partial tarball (loop-64
# lesson: a 117MB corpse passed the [ ! -f ] guard and tar EOF'd) and
# no-ops instantly when the file is already complete.
echo "downloading/resuming flutter $V ..."
curl -sL --retry 8 --retry-delay 5 --retry-all-errors -C - \
  -o "$TARBALL" \
  "https://storage.googleapis.com/flutter_infra_release/releases/stable/linux/flutter_linux_${V}-stable.tar.xz"
echo "tarball size: $(stat -c %s "$TARBALL")"
# Integrity gate BEFORE extract: a truncated archive must never yield a
# half-unpacked SDK dir that the [ ! -d ] guard would then trust.
if ! xz -t "$TARBALL" 2>/dev/null; then
  echo "TARBALL INCOMPLETE — bailing so the retry wrapper resumes it"
  exit 3
fi
if [ ! -d /home/z/flutter/bin ]; then
  rm -rf /home/z/flutter
  mkdir -p /home/z
  tar xf "$TARBALL" -C /home/z/
fi
git config --global --add safe.directory /home/z/flutter || true
/home/z/flutter/bin/flutter --version
echo "FLUTTER INSTALL OK"
