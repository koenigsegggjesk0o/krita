# Feather Reference Research — "Feather: Draw in 3D" (iOS) vs. Feather-Krita (Windows/Android)

> **Status:** RESEARCH COMPLETE (user directive, 2026-09-22, 5-loop-85). Companion
> document to `docs/FEATHER_3D_SCOPE.md` (brush-engine architecture). This doc
> covers the aspects the first scope did NOT: the reference app's full product
> surface, UX patterns, criticisms, platform realities for Windows/Android, and
> the complete gap matrix.
>
> **User mandate (verbatim):** *"intinya saya mau bikin feather 3d sama seperti
> feather 3d tapi kan feather 3d cuma ada di ios nah saya mau buat untuk windows
> dan android … saya gamau ada engine gimmick system gimmick dll yang gimmick"*
>
> Translation of intent: replicate the **category experience** of the iOS app
> "Feather: Draw in 3D" on **Windows + Android**, with **zero gimmicks** — every
> feature real, test-locked, and honestly reported (trust policy §9).

---

## 1. The reference app — verified profile (all from primary pages, 2026-09-22)

| Fact | Value | Source |
|---|---|---|
| Product | **Feather: Draw in 3D** — "Sketch & paint from any angle" | App Store listing |
| Developer | **Sketchsoft Inc., Seoul, South Korea** | feather.art footer |
| Platform | **iPad** (iPad-first; App Store category Graphics & Design; age 4+) | App Store |
| Price | **$14.99 one-time, no subscription** | feather.art + App Store |
| Rating | **4.1 / 5 (109 ratings)**, Apple **Editors' Choice** + design-award mention in reviews | App Store |
| App size | 51.1 MB | App Store |
| Renderer brand | **"Airbreath rendering engine"** (custom, light/shadow interplay) | App Store description |
| Input | Pen + touch; Apple Pencil Pro; **"Finger-pen" mode** for touch-only | feather.art |
| Localization | EN, ES, KO, JA, AR (5+) | feather.art + App Store |
| Version arc | 1.0 (early 2025) → 1.2 (May 2025: AR Viewer) → 1.3 (Jun 2025: Feather Gallery publishing) | 80.lv / cgchannel coverage |
| Community | Discord; Feather Gallery (online interactive 3D showcase) | feather.art |
| Offline | **Fully offline** (no account needed) | feather.art feature list |

**Positioning quote (their words):** "a '3D drawing' app that blurs the line
between drawing and modeling."

---

## 2. Complete feature inventory of the reference app (23 items, evidence-linked)

From the official site + App Store description (both fetched in full):

**Core creation**
1. Spatial canvas — free 3D-space navigation, draw from any perspective
2. Pressure-sensitive **3D brushes** with "styles and patterns"
3. **3D Liquify** — push/pull details in space
4. **Shapes** — straight lines, circles, ellipses, smooth curves
5. **Eraser**
6. **Live Mirror** — symmetric drawing via virtual mirror
7. **Geometric Primitives as drawing surfaces** — spheres, cylinders, cones, rings
8. **Joystick editing** — move/rotate/scale "feels like gaming"

**Presentation / finishing**
9. **Lighting** — one-tap light + drop shadow
10. **Post-processing FX** — grain, depth of field, glow, pixelation, toon shading
11. **Camera story shots** — capture + sequence shots from angles

**Workspace / assets**
12. **Import Reference** — images AND 3D models
13. **Clipboard** — reference images in a floating separate window
14. **Background Image** — color/image behind the 3D artwork
15. **Folders** — organize drawings
16. **Data Backup**
17. **Fully offline**

**Output / sharing**
18. **Export to 3D formats** (use in other tools)
19. **AR View** — view creations in real-world spaces (v1.2)
20. **Share via 3D viewer link** + **Feather Gallery** publishing (v1.3)
21. **Finger-pen mode**
22. Community: Discord + gallery + docs + video tutorials
23. Multi-language UI (EN/ES/KO/JA/AR)

---

## 3. What the reference app gets WRONG (from the 4.1★ critical reviews)

The most substantive public review ("Solid 4, not 5", Ronbo13, 2025-06-29,
App Store) — an honest avoid-list for us:

1. **Weak undo affordances** — "no 2-finger tap to undo… any way to undo except
   tapping the arrow." → We already have 50-level stroke undo; add standard
   platform gestures (Ctrl+Z, 2-finger tap on touch) as a first-class rule.
2. **No left-handed UI flip.** → UI mirroring toggle belongs in our settings
   from day one.
3. **No tooltips, obscure iconography.** → Every control gets a tooltip/label.
4. **Straight-line mode entangled with "curve prettification"** — the engine
   snaps to curves ~95% of the time; endpoints can't be moved; the developer
   reportedly said it "would need an engine revamp." → Our shape tools must
   have EXPLICIT modes (line/curve toggle), draggable endpoints, and no hidden
   snapping. This is exactly the "no gimmick" UX rule.
5. Fat-pen endpoint artifacts ("pie-with-a-slice-out" caps) → our stroke ribbon
   caps already have dedicated geometry (loop-53/54 work); keep test-locked.

**Review verdict:** "very cool app… but feels like a beta" — the market reward
goes to whoever ships the same category with polish.

---

## 4. Gap matrix — reference app vs. Feather-Krita TODAY

Status key: ✅ HAVE (shipped) · 🟡 PARTIAL · ❌ MISSING (planned) · ➖ N/A-by-platform

| # | Reference feature | Our status | Evidence / plan |
|---|---|---|---|
| 1 | Spatial canvas + free navigation | ✅ | camera_controller (orbit/zoom), canvas_widget raycast pipeline |
| 2 | Pressure-sensitive 3D brushes | ✅ **+ our differentiator** | Stroke ribbons textured by the REAL Krita engine; 16 families via ABI v2 (scope doc §4) — far beyond "styles and patterns" |
| 3 | 3D Liquify | ✅ | stroke_manager: push/pull/twist/inflate/deflate/smooth (loop-51/52) |
| 4 | Shapes (line/circle/ellipse/curve) | ❌ | NEW: explicit-mode shape tools (see §3.4 — modes must be explicit, endpoints draggable) |
| 5 | Eraser | ✅ | real-engine eraser presets + destination-out composite |
| 6 | Live Mirror | ✅ | mirror sign vectors in stroke_manager |
| 7 | Geometric primitives to draw on | ✅ **superset** | GuideSurfaceType: sphere, cylinder, cone, ring, plane, customCurve (6 vs their 4) |
| 8 | Joystick move/rotate/scale | 🟡 | JoystickWidget exists (floating 3D manipulation); audit scale-mode coverage + add rotate handle parity |
| 9 | Lighting + drop shadow | 🟡 | light_rig + presets (noon/golden hour/rim); **shadows not yet implemented** |
| 10 | Post-processing FX (grain/DoF/glow/pixelation/toon) | ❌ | NEW workstream — pure-Dart post pipeline on the rasterizer output (deterministic, test-locked) |
| 11 | Camera capture + story shots | 🟡 | GIF/MP4 exporters exist; add shot-capture + sequence storyboard UX |
| 12 | Import reference (images + 3D models) | ❌ | image: trivial overlay; 3D: rides F4 mesh import (scope doc §9) |
| 13 | Clipboard floating window | ❌ | Flutter multi-window/overlay; desktop-first, Android picture-in-picture later |
| 14 | Background image/color | ❌ | render-pass change, small |
| 15 | Folders | 🟡 | recent_projects exists; add folder grouping |
| 16 | Data backup | 🟡 | .feather project files are portable; add explicit backup/export-all flow |
| 17 | Fully offline | ✅ | no account, no network dependency (and honest: our CI/gallery features stay optional) |
| 18 | Export 3D formats | ✅ **superset** | GLTF (+ GIF/MP4/.feather) — we also export ANIMATED outputs |
| 19 | AR view | ❌ / ➖ | Android: ARCore (feasible, later); Windows: N/A — substitute: 3D viewer export |
| 20 | Share via 3D viewer link + gallery | ❌ | needs hosting; model-viewer web export is the no-backend v1 (share a .glb + viewer) |
| 21 | Finger-pen mode | ❌ | touch pressure simulation (velocity-based) — small, testable |
| 22 | Community/docs | 🟡 | worklog+HANDOVER are dev-side; user-facing docs/tutorials = release blocker for parity |
| 23 | Multi-language | ❌ | Flutter i18n from the start of the UX phase (5 languages is their bar) |

**Score today: 9 HAVE/PARTIAL strong, 8 missing-but-planned, 2 platform-N/A.**
Our two structural advantages they cannot match: (a) **real Krita brush
engines** (their brushes are custom and closed), (b) **Windows + Android +
Linux** (they are iPad-only).

---

## 5. Platform aspects for Windows & Android (not covered by scope doc v1)

| Aspect | Windows | Android |
|---|---|---|
| Pen input | Windows Ink / WM_POINTER (pressure, tilt, barrel) — Flutter PointerData carries pressure; tilt needs platform channel | Stylus via MotionEvent (pressure, tilt for S Pen/Pencil-class devices); finger = Finger-pen mode |
| Undo gesture | Ctrl+Z / Ctrl+Shift+Z (standard) | 2-finger tap (the gesture the reference app lacks) |
| AR | ➖ not applicable | ARCore — but 192 MB APK + engine runtime means AR ships LATE, honest badge if absent |
| Precision pointer | Mouse+pen dual input, hover states | Touch-first, 44px targets, safe areas |
| Distribution | Signed zip (self-contained, v0.49 closure proves it runs on clean hosts) | APK (emulator-smoke-proven boot + soak) |
| Performance | Desktop CPU software rasterizer fine at 4k atlas | Mobile: dirty-rect discipline + 1k–2k atlas caps (scope doc §8) |

---

## 6. Legal & branding honesty (no-gimmick includes this)

- **Category, not clone:** we implement the same FEATURE CATEGORY with our own
  architecture, code, and visual design. No assets, iconography, copy, or UI
  expression from Sketchsoft's app is reused. Feature ideas are not
  copyrightable; expression is — we stay on the right side.
- **Name collision:** the public-facing product name should eventually be
  distinct from "Feather: Draw in 3D" (their trademark territory). Internally
  "Feather-Krita"/"Feather-3D" stays (private repo, honest history). Flag for
  the user to pick a public name before any public store listing.
- **Krita engine licensing:** unchanged — GPL via our wrapper, source mirror
  untouched (standing rule).

---

## 7. Roadmap integration (extends FEATHER_3D_SCOPE.md §9)

F1–F5 (brush engines → mesh era) remain the spine. New workstreams slot in:

- **F3.5 — Parity UX pack** (small, high-value): explicit shape tools with
  draggable endpoints, undo gestures (Ctrl+Z + 2-finger tap), tooltips on all
  controls, left-handed UI flip, finger-pen mode, background image/color.
  Rationale: every item is a known complaint about the reference app (§3).
- **F5.5 — Finishing pack:** drop shadows (light rig), post FX (grain, DoF,
  glow, pixelation, toon — pure-Dart post passes), shot capture + sequence
  storyboard (reuses GIF/MP4 exporters).
- **F6 — Workspace pack:** folders, explicit backup flow, reference image
  import + clipboard window, 3D model reference (rides F4 mesh import).
- **F7 — Sharing:** GLB + web-viewer export (model-viewer style, offline-first),
  optional hosted gallery later (only with user decision — no silent network).
- **AR (Android, optional F8):** ARCore viewer of exported scenes, after the
  performance work settles.

Every feature ships with the no-gimmick contract: real implementation,
widget/golden tests in the 223+ suite, honest UI labeling (REAL/PORTABLE/
FALLBACK badges unchanged), and CI-visible smoke coverage where engine-bound.

---

## 8. Sources (fetched 2026-09-22)

- Official site (full text): https://www.feather.art — product, features, price,
  gallery, community, i18n, "Made by Sketchsoft Inc. in Seoul".
- App Store listing: apps.apple.com/us/app/feather-draw-in-3d/id6737254232 —
  description ("Airbreath rendering engine"), $14.99, 4.1★/109, Editors'
  Choice, 51.1 MB, iPad, full critical review (Ronbo13).
- Creative Bloq (2025-04-08): "What's Feather app, the new 3D tool for iPad
  artists?" — category context, spatial canvas, 3D-format export.
- 80.lv (2025-05-13): "Feather 3D 1.2 Is Out Now" — AR Viewer introduction.
- CG Channel (2025-06-06): "Sketchsoft releases Feather 1.3" — Feather Gallery
  publishing (interactive online 3D).
- krita-artists.org (2025-11-01): professional demand signal — artists want
  Feather 3D portability ("only because I would like to have Fresco and
  Feather 3D on the go") — validates the Windows/Android mandate.
- Local app evidence: lib/engine/guide_surface.dart (6 surface types),
  lib/widgets/joystick_widget.dart (exists), lib/engine/stroke_manager.dart
  (liquify+mirror), lib/io/* (gltf/gif/mp4/.feather exporters),
  worklog 5-loop-74 audit (Feather-3D modules all real, no gimmicks).
