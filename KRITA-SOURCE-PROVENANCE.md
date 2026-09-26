# Krita Source Provenance — VERIFIED

**User requirement (2026-09-26):** source code MUST come from krita.org, NOT from
github.com/KDE/krita. This file documents the verified provenance.

## What was replaced and when

- **Date:** 2026-09-26
- **Removed:** previous `krita-source/` import (was cloned from
  `github.com/KDE/krita` tag v6.0.4 — commit `87041733` in this repo's history).
  That origin violated the user's explicit instruction and has been discarded.
- **Replaced with:** the official source tarball linked from the krita.org
  download page (krita.org → Download → "Source code" → download.kde.org).

## Verified download

- **URL:**
  `https://download.kde.org/stable/krita/6.0.4/krita-6.0.4.tar.xz`
  (the canonical host that krita.org's download page links to for source)
- **Size:** 206,806,924 bytes (matched `Content-Length` exactly)
- **Official sha256 (served by download.kde.org):**
  `ef73e57fe20fa5bd896d53d4af6eb254e5ade83e6a39b47273dd320f9823ab4b`
- **Local sha256 of downloaded file:**
  `ef73e57fe20fa5bd896d53d4af6eb254e5ade83e6a39b47273dd320f9823ab4b`
- **Result: MATCH — tarball is authentic and unmodified.**

## Verification of extracted tree

- Top-level dir `krita-6.0.4/` extracted and renamed to `krita-source/`.
- `krita-source/CMakeLists.txt`: `set(KRITA_VERSION_STRING "6.0.4")`
- File count after extraction: **12,331 files** (578 MB).
- No `.git` directory inside (pure release tarball, not a git clone).
- Note: the tarball does include KDE infra dirs `.github/` and `.gitlab/`
  — KDE ships these inside the official release tarball itself; their
  presence is expected and is NOT evidence of a git-clone origin.
