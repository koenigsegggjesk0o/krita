#!/usr/bin/env python3
"""Simulate the CI PowerShell patch logic on /tmp copies (NOT krita-source)."""
import re, pathlib

patches = [
    {
        "file": "KisDatabaseTransactionLock.h",
        "anchor": "    void commit();",
        "lines": [
            "    bool try_lock()",
            "    {",
            "        if (m_transactionStarted) {",
            "            return false;",
            "        }",
            "        lock();",
            "        return true;",
            "    }",
        ],
    },
    {
        "file": "KoShapeBulkActionLock.h",
        "anchor": "    void unlock();",
        "lines": [
            "    bool try_lock()",
            "    {",
            "        lock(); // bulk action cannot fail; MSVC Lockable completeness only",
            "        return true;",
            "    }",
        ],
    },
]

base = pathlib.Path("/tmp/patch-test")
for p in patches:
    f = base / p["file"]
    text = f.read_text(encoding="utf-8")
    if re.search(r"try_lock\s*\(", text):
        print(f"SKIP (already patched): {p['file']}")
        continue
    if p["anchor"] not in text:  # .Contains equivalent
        print(f"FATAL: anchor not found in {p['file']}")
        raise SystemExit(1)
    insert = p["anchor"] + "\r\n" + "\r\n".join(p["lines"])
    new_text = text.replace(p["anchor"], insert)  # .Replace equivalent
    f.write_text(new_text, encoding="utf-8", newline="")
    print(f"PATCHED: {p['file']}")
print("SIMULATION COMPLETE")
