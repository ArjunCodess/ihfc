# Single CareBot pack implementation plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Keep one CareBot firmware sketch and one usable project pack, without an `output/showcase` folder.

**Architecture:** Move the useful showcase files to `output/CareBot`. Retain only `output/CareBot/CareBotESP32/CareBotESP32.ino` as firmware, with `output/CareBot/WIRING.md` as the detailed wiring map. Point tests, compile script, HTML, and documentation at those files; refresh the packaged ZIP and checksums.

**Tech Stack:** Arduino-ESP32 3.3.5, PowerShell, C++17 regression mock, Markdown, Git

---

### Task 1: Consolidate the pack

**Files:**
- Move: `output/showcase/CAREBOT-SHOWCASE/*` to `output/CareBot/*`
- Remove: `output/arduino/CareBotESP32/CareBotESP32.ino` and duplicate guide files after preserving the firmware in `output/CareBot/CareBotESP32/CareBotESP32.ino`
- Remove: `output/showcase/CAREBOT-SHOWCASE.zip` after replacing it with `output/CareBot.zip`

- [x] **Step 1: Confirm both firmware files are identical**

Run a normalized-content comparison. Expected: `True`.

- [x] **Step 2: Move the existing pack**

Use `git mv` on each tracked file from `output/showcase/CAREBOT-SHOWCASE` to `output/CareBot`, retaining all useful documents, tests, scripts, and evidence. Remove only the duplicate firmware and redundant `output/arduino` guide copies, whose content is already in `TECHNICAL-GUIDE.md` and `REVIEW.md`.

### Task 2: Repair internal links and test paths

**Files:**
- Modify: `output/CareBot/tests/test.cpp`
- Modify: `output/CareBot/START-HERE.md`
- Modify: `output/CareBot/SHOWCASE.html`
- Modify: `output/CareBot/compile.ps1`
- Modify: `docs/CAREBOT-FINAL-ESP32.tex`

- [x] **Step 1: Update project references**

Use `output/CareBot/CareBotESP32/CareBotESP32.ino` in repository-level documentation. Keep local links such as `CareBotESP32/CareBotESP32.ino` within the pack.

- [x] **Step 2: Run compile and test checks**

Run `arduino-cli compile --fqbn esp32:esp32:esp32doit-devkit-v1 --warnings all output/CareBot/CareBotESP32` and warning-clean C++17 compilation of `output/CareBot/tests/test.cpp`. Expected: both compile without diagnostics.

### Task 3: Add DevKit V1 labels to canonical wiring

**Files:**
- Modify: `output/CareBot/WIRING.md`
- Modify: `output/CareBot/TECHNICAL-GUIDE.md`

- [x] **Step 1: Add board-label column**

Map GPIO25/26/27/14/33 to `D25/D26/D27/D14/D33`; GPIO17/16 to `TX2/RX2`; GPIO23/34/35/21/22/32 to `D23/D34/D35/D21/D22/D32`. Explain that these labels describe the common 30-pin DOIT ESP32 DevKit V1 and that the actual board silkscreen takes precedence.

- [x] **Step 2: Keep wiring in one place**

Replace the duplicate technical-guide pin table with a pointer to `WIRING.md`. Preserve its calibration and safety information.

### Task 4: Refresh artifacts and publish

**Files:**
- Modify: `output/CareBot/SHA256SUMS.txt`
- Create: `output/CareBot.zip`

- [x] **Step 1: Refresh checksums and ZIP**

Calculate SHA-256 for every pack file except `SHA256SUMS.txt`, then create a ZIP from `output/CareBot`. Expected: ZIP entries match the new pack and no old showcase sketch is included.

- [x] **Step 2: Verify one source of truth**

Run `rg --files output -g '*.ino'`, `git diff --check`, and check all local Markdown/HTML links. Expected: exactly one `.ino` file and no `output/showcase` path.

- [x] **Step 3: Commit and push**

Commit the change on `main` and push `origin main`; verify `HEAD` equals `origin/main` and the worktree is clean.
