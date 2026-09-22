# HANDOVER — Feather-Krita

> **STOP NOTICE (2026-09-22 ~11:30 Asia/Jakarta):** The autonomous cron loop
> (`cron-agent-loop-*`, job id `395817`, every 30 min) has been **DELETED** by the
> user so they can switch agents. Everything below is the full context a new agent
> needs to pick up where the previous one stopped. To resume the loop, recreate the
> cron job — full payload + schedule are captured in §8 below.

This file is the single source of truth for a new agent taking over Feather-Krita.
Read it top to bottom before doing anything.

---

## 1. What this project is

**Feather-Krita** = a Flutter host app that drives the **real Krita v6.0.4 brush
engine** via an FFI bridge. The headline rule:

> **Krita source is NEVER modified.** Only these surfaces may change:
> - builder CI workflow (`krita-build.yml`, `build-app.yml`)
> - the thin C ABI wrapper `native/krita_bridge/krita_bridge_real.cpp`
> - the Dart smoke tool / app code
> - apt / package lists in the builder
>
> Everything else (krita source tree, third-party deps) is treated as read-only.

The wrapper exposes a tiny C ABI (`krita_brush_*`, `krita_preset_*`, …) that the
Flutter app calls through `dart:ffi`. A fallback bridge (`krita_bridge_portable.cpp`)
reimplements the same ABI for engines/platforms where the real one isn't built yet,
so the app degrades gracefully.

---

## 2. Repos & access

| Repo | Role | Default branch |
|------|------|----------------|
| `koenigsegggjesk0o/krita` | **App repo** (Flutter source, wrapper, smoke, workflows that consume the engine) | `feather-krita-flutter` |
| `koenigsegggjesk0o/feather-krita-build` | **Builder repo** (CI that compiles the real Krita engine + smoke; mirror of `native/**` + `worklog.md`) | `main` |

- **GitHub token:** `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` (used by the loop; treat as a secret).
- **Local app checkout:** `/home/z/fkr-step1` (on branch `feather-krita-flutter`).
- **Flutter SDK:** `/home/z/flutter/bin/flutter`.
- **Builder repo is NOT cloned locally** — interact with it via the GitHub Contents API (the loop mirrors `native/**` files into it and syncs `worklog.md`). Do NOT clone `krita-source/` locally (disk is ~8GB free; the source tree is huge and lives only in CI).

---

## 3. Current state (as of handover)

**DONE — full chain green and released:**

- `set_param` C ABI contract is **CI-proven on real Krita v6.0.4 engines**:
  - **rebuild #3** = `krita-build` run **35678879422**, all 4 legs (Linux, Windows,
    android-x86_64, android-arm64) **SUCCESS**, smoke log: *"SMOKE OK — real Krita
    bridge end-to-end"* on both fixtures.
  - Proven semantics: self-configuring replace-arm (map entry 0 edited in place,
    count flat, read-back), live opacity/flow/hardness effects, unknown-key append
    == +1 with read-back, argument rejections.
- `build-app` run **35680866715** — 5/5 **SUCCESS** (~9 min); the NEW engine (with
  `set_param`) was staged into all three bundles. App HEAD at the time = `19b2244`.
- **Emulator smoke** (`35681273110` family) **PASSED** (boot + 90s soak on the
  `set_param` APK).
- **Release `v0.46-live-param-editing`** published on the app repo (release id
  `393430486`, target `feather-krita-flutter`). Three assets, all
  `state=uploaded` & verified:
  - `feather-krita-linux-real-engine.zip`
  - `feather-krita-windows-real-engine.zip`
  - `feather-krita-android-real-engine.apk` (~183 MB)
- App repo HEAD at handover: `f0af225` ("5-loop-72 addendum — FULL CHAIN GREEN,
  v0.46-live-param-editing RELEASED").
- Dart gate: `flutter analyze` = 0 errors / 0 warnings / 75 pre-existing infos.
- **Krita source untouched throughout** — wrapper + smoke + Dart + workflow surfaces only.

**No pending CI.** The roadmap item "(f) Preset loading upgrade (paintop-settings
level params)" is complete and shipped.

**UPDATE 2026-09-22 ~10:40 UTC (5-loop-78) — v0.47-curve-editing RELEASED;
roadmap milestone (i) CLOSED.** Sensor-curve editors shipped end-to-end:
`krita_brush_get_curve`/`set_curve` ABI (5-loop-76 @ ee0eaec) + smoke
fixture-model fix and the full Dart side — getCurve/setCurve bindings,
EditorState plumbing, sensor-curves section + curve editor dialog
(5-loop-77 @ b564a12; fixture lesson encoded as §6.6). Chain: krita-build
**35712006206** 4/4 SUCCESS (~27 min; SMOKE OK on both fixtures, all curve
gates green incl. the eraser EMPTY-form set_curve rejection), build-app
**35715078040** SUCCESS (~7 min), emulator smoke **35715701916** SUCCESS.
Release **v0.47-curve-editing** = release id **393638347** (3/3 assets
uploaded: linux zip 47,510,096 B / windows zip 40,232,626 B / android apk
191,962,550 B). App HEAD at release: `42e5179` (+ the 5-loop-78 worklog
commit). Builder mirror current @ `432175c` (blob-SHA verified 6/6 vs app
HEAD). Release script convention: `/home/z/my-project/scripts/release_v47.py`.
Dart gate: 0 errors / 0 warnings / 75 pre-existing infos. NEXT candidates
(5-loop-79): honesty-gap backlog (active-engine badge via
`krita_brush_version()` — currently unused from lib/, Linux bundle runtime
closure, Linux portable fallback) or a fresh §5 feature survey; the CI
re-point offer (krita-build.yml → upstream KDE clone) remains PENDING USER
DECISION.

---

## 4. The autonomous loop convention (resume this if you re-enable the cron)

Every tick the agent:

1. **Read** `/home/z/fkr-step1/worklog.md` (tail ~150 lines) for the handoff.
   Next Task ID = `5-loop-N` (increment N from the last entry).
   **25-min mtime guard:** if the worklog was written < 25 min ago, skip (another
   loop is in progress). This guard may be **superseded** with evidence: same
   continuous session (context-continuation handoff), tree clean, no concurrent
   agent — there's precedent (5-loop-65/69/71/72 all superseded).
2. **Check CI on the builder repo:**
   ```bash
   curl -s -H 'Authorization: token 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' \
     'https://api.github.com/repos/koenigsegggjesk0o/feather-krita-build/actions/runs?per_page=5' \
   | python3 -c 'import sys,json;[print(r["id"],r["name"][:35],r["status"],r.get("conclusion"),r["head_sha"][:7]) for r in json.load(sys.stdin)["workflow_runs"]]'
   ```
3. **Branch on status:**
   - `IN_PROGRESS` → skip + brief worklog note.
   - `FAILURE` → `curl -sL` the failing job's logs URL, find `##[error]`, fix the
     allowed surface (workflow / wrapper / smoke / apt — **never Krita source**),
     commit to the app repo, re-dispatch `krita-build`.
   - `SUCCESS` → advance the roadmap.
4. **Roadmap** (priority order — items a–f are DONE; what remains is in §5):
   - (a) ✅ v0.20 real-engine Linux tag
   - (b) ✅ Windows real engine (MSVC build, .dll bundle)
   - (c) ✅ Android real engine (NDK cross-build per ABI)
   - (d) ✅ Self-contained Linux bundle (`patchelf --set-rpath $ORIGIN`)
   - (e) ✅ build-app diagnostic cleanup
   - (f) ✅ Preset loading upgrade — paintop-settings-level params + live editing
5. **`flutter analyze`** gate (deprecation infos OK; fix only ERRORS):
   ```bash
   cd /home/z/fkr-step1 && /home/z/flutter/bin/flutter analyze --no-fatal-infos --no-fatal-warnings
   ```
6. **Append worklog entry:** `---` separator, `Task ID: 5-loop-N`, `Agent:`, `Task:`,
   `Work Log:` (concrete steps), `Stage Summary:` (results + NEXT for the next tick).
7. **Commit + push app repo:**
   ```bash
   cd /home/z/fkr-step1 && git add -A && \
     git -c user.name='Feather-Krita Bot' -c user.email='bot@feather-krita.local' \
     commit -m 'autonomous loop: <desc>' && git push origin feather-krita-flutter
   ```
8. **Sync builder repo** via GitHub Contents API for any `native/**` files that
   changed (code mirrors trigger `build-app`; worklog-only syncs are silent).
   Legacy pipelines firing on `native/**` matches are informational.

**Don't give up.** Fix and retry until SUCCESS — that's the standing directive.

---

## 5. Next milestones (candidates, in rough priority order)

- **Krita menu / tab feature surface survey** — which curated-ABI capabilities
  remain unexposed? Candidates: color/pigment API, brush-tips mode, opacity/flow
  curve editors. Pick one, expose it through the wrapper + Dart FFI + a panel UI.
- **Thumbnail caching** for the preset picker — only if scroll perf regresses;
  currently the picker loads presets lazily and there's no on-disk cache.
- **Feather-3D engine** — a separate goal on the original list; not started.
- **Mirror-sync double-dispatch hygiene** — `native/**` mirrors currently fire
  `build-app` plus the legacy pipelines; the informational-only stance works but
  could be tightened to avoid duplicate runs.

When you pick one, follow the loop convention: dispatch `krita-build` if the
wrapper changed, then `build-app`, then emulator smoke, then tag a release.

---

## 6. Hard-won lessons (read these — they cost real CI time)

### 6.1 `krita_brush_set_param` ABI semantics (empirically proven on real engine)

- `set_param(key, value)` is **replace-or-append** on the param map of record
  (preserves document order) AND applies a live engine effect for consumed keys.
- **`Krita/opacity` is NOT a pre-existing entry of the projected `<param>` map**
  — `loadPreset` reads it from XML settings, not the projection. So an opacity
  edit **APPENDS** to the map (count += 1), even though the live effect applies.
  Do NOT write a smoke assertion that pins opacity-edit count as flat.
- `FlowValue` and `hardness` are **fixture-dependent** — they may or may not
  pre-exist in a fixture's map. Any count assertion must use a **probe-local
  baseline** (capture `before` immediately before the probe, not before the whole
  edit block).
- The **replace arm** is best tested self-configuring: pick map entry 0 at runtime
  (guaranteed to exist), edit it to `"<v0>-edited"`, assert count flat + read-back.
- The smoke's unknown-key probe must use a key **guaranteed absent** from both
  fixtures — `FeatherKrita/Probe` is namespaced outside anything Krita writes.
  (Earlier probe used `ColorSource/Type`, which basic-5's map already carries →
  replace arm → +1 assertion failed.)

### 6.2 C++ wrapper gotcha

- Don't shadow a name in the same scope. The `set_param` function had
  `const std::string v = value;` (map-of-record capture) AND `double v = 0.0;`
  (numeric parse) in the same scope — GCC/MSVC/NDK clang all rejected it
  ("conflicting declaration" / "redefinition"). Rename one (`v` → `recorded`).
  The build-app / legacy pipelines never compile the `_real_` bridge, so this
  only surfaces in `krita-build` — a green build-app does NOT prove the real
  bridge compiles.

### 6.3 GitHub API traps (for release scripts)

- **`urllib` artifact download 403s** — GitHub redirects to Azure storage and
  `urllib` FORWARDS the `Authorization` header into the signed redirect URL → 403.
  Use `curl -sL` (drops auth cross-host) + `size` verification + retries.
- **Asset upload 404 right after release creation** — the new release tag
  materializes asynchronously; uploads to `uploads.github.com` / `.../assets`
  404 for a beat. Make the release script **idempotent** and re-runnable; it
  must tolerate a 404 on the tag pre-lookup and skip already-uploaded assets.
- **`curl` without `--fail`/`-f` swallows HTTP errors** — always use `-f` so a
  failed upload propagates instead of printing "COMPLETE" on a 404 body.
- **Artifact zips may contain a nested payload** (`.zip` inside `.zip`, or
  `.apk` inside a zip) — unpack before uploading.
- **api() must tolerate 404** on the release-tag pre-lookup (tag may not exist
  yet on first run).

### 6.4 Agent toolchain display sanitizer (5-loop-73 — verify bytes, not text)

- The agent toolchain's rendered output STRIPS the two-char sequence `[m`
  from displayed text. YAML `branches: [main]` displays as `branches: ain]`;
  `[main, feather-krita-flutter]` displays as `ain, feather-krita-flutter]`.
  The 5-resume "corrupted push branch filter" finding was this artifact —
  byte-level verification (Contents-API base64 codepoint dumps, `od -c` on
  raw.githubusercontent.com bytes) proved ALL builder branch filters were
  healthy. Do NOT "fix" phantom `[m`-loss corruption anywhere.
- Protocol: before patching any "weird" text found in tool output, dump
  codepoints programmatically (`[hex(ord(ch)) for ch in line]`) or `od -c`
  the raw bytes. Text rendering is NOT ground truth in this environment.

The current release script is `/home/z/my-project/scripts/release_v46.py`
(builder mirror `01b2711`) — it encodes all of the above. Follow its convention
for the next release.

### 6.5 Engine runtime version string reports 5.3.4 (source is v6.0.4 — verified)

- `KritaVersionWrapper::versionString()` in the built engine reports
  **5.3.4**, not 6.0.4. This is UPSTREAM's own CMakeLists.txt logic at tag
  `v6.0.4` (two `set(KRITA_VERSION_STRING ...)` blocks — lines ~134/140 —
  the later 5.3.x block wins in this build configuration). The mirror
  `krita-source/` is byte-identical to upstream `v6.0.4` (12,325 blobs,
  tree-diff via git blob SHAs, 5-loop-74 audit) — do NOT "fix" or relabel
  this; report both facts: source = v6.0.4 (proven), runtime string =
  upstream behavior.

### 6.6 Projected sensor-curve entries come in TWO forms (5-loop-77 — CI-proven)

- The engine's projected param map carries `"<CurveOption>Sensor"` keys
  in **two shapes**: a REAL curve
  (`<!DOCTYPE params> <params id="pressure"> <curve>0,0;…;1,1;</curve>
  </params>`) and the **EMPTY params form**
  (`<!DOCTYPE params> <params id="pressure"/> ` — NO `<curve>` child).
- Fixture ground truth (raw preset XML, CDATA + attribute order verified
  byte-level): basic-5 `FlowSensor` = real curve + `FlowUseCurve=true`;
  `OpacityUseCurve=false`; stock_eraser_circle `FlowSensor` = EMPTY form
  + `FlowUseCurve=false`. The 5-loop-76 note "eraser ships ZERO curve
  params" was a regex artifact (the eraser XML writes
  `<param type="string" name="FlowSensor">` — type-before-name — which
  beat a name-first probe regex; CDATA broke a second naive pattern).
- Consequences: (a) `get_curve` on the eraser returns NON-NULL (the
  empty form) — never pin sensor ABSENCE without a byte-level fixture
  probe; (b) the empty form is NOT `set_curve`-writable by contract
  (the validator requires a `<curve>` child with >= 2 points) — the
  smoke's honest gate for it is the -1 rejection; (c) probe scripts
  must be CDATA-aware and attribute-order-agnostic (protocol 6.4:
  verify bytes, not patterns).
- The Dart curve editor handles both forms (empty form → default linear
  curve seeded for editing, original `id` preserved).

---

## 7. Quick verification commands (sanity check on resume)

```bash
# App repo state
cd /home/z/fkr-step1 && git log --oneline -3 && git status --short

# Latest builder CI
curl -s -H 'Authorization: token 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' \
  'https://api.github.com/repos/koenigsegggjesk0o/feather-krita-build/actions/runs?per_page=5' \
| python3 -c 'import sys,json;[print(r["id"],r["name"][:35],r["status"],r.get("conclusion"),r["head_sha"][:7]) for r in json.load(sys.stdin)["workflow_runs"]]'

# v0.46 release assets (should be 3, all state=uploaded)
curl -s -H 'Authorization: token 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' \
  'https://api.github.com/repos/koenigsegggjesk0o/krita/releases/393430486/assets' \
| python3 -c 'import sys,json;[print(a["name"],a["state"],a["size"]) for a in json.load(sys.stdin)]'

# Dart gate
cd /home/z/fkr-step1 && /home/z/flutter/bin/flutter analyze --no-fatal-infos --no-fatal-warnings
```

---

## 8. How to re-enable the autonomous cron loop

The deleted job was `395817`, name **"Feather-Krita autonomous build loop (every 30 min)"**.
To recreate it (via the `cron` tool, `action=create`):

- **name:** `Feather-Krita autonomous build loop (every 30 min)`
- **schedule:** `kind=fixed_rate`, `expr=1800` (seconds), `tz=Asia/Jakarta`
- **payload:** `kind=agentTurn`, **message** = the full loop task book (the
  DO-THIS-EVERY-RUN list from §4, prefixed with the project dir / repos / token /
  Flutter SDK / current-state one-liner). The previous payload is preserved in
  the conversation history if you need it verbatim; the §4 list above is the
  canonical content.

When recreating, update the "CURRENT STATE" one-liner to reflect whatever the
new agent has done since this handover.

---

## 9. Standing user directives (do not violate)

- **Never modify Krita source.** Wrapper / smoke / Dart / workflow / apt only.
- **Never give up** — fix and retry until SUCCESS.
- **User timezone:** `Asia/Jakarta`. Interpret relative dates/times in this TZ.
- **Trust policy (5-loop-74 user directive):** the user does NOT trust prior
  agents on faith. Verify claims with PRIMARY evidence (CI job logs, release
  asset bytes, git blob SHAs, `od -c`/codepoint dumps), never from worklog
  narrative alone. A full no-gimmick audit of every non-Krita surface
  (bridge, Feather-3D engine, CI system, released bundles) was executed at
  **5-loop-74** — all seven audit areas PASSED (see that worklog entry for
  the evidence trail). Re-audit any surface you touch.
- **Source provenance re-confirmed at 5-loop-75 (user question "is my
  krita-source real?"):** three-way blob-SHA triangulation — app-repo
  `main:krita-source/` (12,325 blobs, 0 modified, 0 injected, 0 gitlinks
  both sides) == `github.com/KDE/krita` tag `v6.0.4` == `invent.kde.org`
  raw bytes (CMakeLists.txt blob SHA `aea85321575e…` identical in all
  three). Upstream v6.0.4 is a genuine annotated tag (Dmitry Kazakov,
  2026-09-10, commit `e7e52a72ed37`). Mirror import commits were authored
  by the user's own account. Rerun anytime:
  `GITHUB_TOKEN=… python3 /home/z/my-project/scripts/audit_source_provenance.py`
- The user expects long autonomous runs; keep the worklog self-contained so any
  tick can resume from it alone.
