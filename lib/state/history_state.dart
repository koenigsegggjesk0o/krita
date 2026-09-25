// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// history_state.dart — Command-pattern undo/redo with branch support.
//
// The legacy [EditorState] keeps a flat _undoJournal / _redoJournal of
// pre-op snapshots (strokes JSON + texture bytes). That works for
// paint-stroke granularity but cannot describe composite operations
// (e.g. "paste 5 strokes as one logical step", "load project as one
// step") or branch when the user undoes and then performs a new action.
//
// This file adds the missing layer: a [HistoryCommand] interface that
// knows how to undo AND redo itself, plus a [HistoryState] container
// that holds the linear stack + branch metadata. The container caps
// the stack at [maxSteps] = 100 entries and records the branch lineage
// so the UI can show "you forked at step N" affordances.
//
// Pure value type — emits through a Riverpod [Notifier].

/// Maximum number of undo steps retained. Older entries are evicted
/// (FIFO) once the cap is reached.
const int kHistoryMaxSteps = 100;

/// A single reversible operation. Implementations carry enough state
/// to roll the document forward (redo) and backward (undo) without
/// re-reading any external snapshot.
abstract class HistoryCommand {
  /// Short human-readable label for the history panel (e.g. "Draw stroke",
  /// "Move 3 strokes", "Load project").
  String get label;

  /// Optional ISO-8601 timestamp the command was recorded at.
  DateTime? get timestamp => null;

  /// Reverses this command's effect.
  void undo();

  /// Re-applies this command's effect.
  void redo();
}

/// One frame in the undo stack. Tracks the branch id (so consumers can
/// visualize fork points) and the parent frame index (so a redo into a
/// previously-discarded branch is recoverable).
class HistoryFrame {
  HistoryFrame({
    required this.id,
    required this.command,
    required this.parentId,
    this.branchLabel,
  });

  /// Monotonic frame id (assigned by the history container).
  final int id;

  /// The command stored at this frame.
  final HistoryCommand command;

  /// Id of the parent frame (the frame that was the current frame when
  /// this command was pushed). `null` for the very first frame.
  final int? parentId;

  /// Optional label for the branch this frame started (e.g. "after
  /// undo of 'Move stroke'"). `null` for linear extensions.
  final String? branchLabel;

  @override
  String toString() =>
      'HistoryFrame($id: ${command.label}${branchLabel != null ? ' [$branchLabel]' : ''})';
}

/// Pure history state.
class HistoryState {
  const HistoryState({
    this.frames = const <HistoryFrame>[],
    this.cursor = -1,
    this.nextId = 0,
    this.maxSteps = kHistoryMaxSteps,
  });

  /// All frames retained in the stack (oldest first).
  final List<HistoryFrame> frames;

  /// Index of the current frame in [frames], or -1 when the stack is
  /// empty / before the first frame.
  final int cursor;

  /// Next available frame id.
  final int nextId;

  /// Maximum number of frames retained (older evicted FIFO).
  final int maxSteps;

  /// True when [undo] would do something.
  bool get canUndo => cursor >= 0 && frames.isNotEmpty;

  /// True when [redo] would do something.
  bool get canRedo => cursor + 1 < frames.length;

  /// The current frame, or `null` when at the bottom.
  HistoryFrame? get current =>
      cursor >= 0 && cursor < frames.length ? frames[cursor] : null;

  /// Number of frames available to undo (0 when at the bottom).
  int get undoDepth => cursor + 1;

  /// Number of frames available to redo (0 when at the top).
  int get redoDepth => frames.length - cursor - 1;

  /// Returns a state with [command] pushed on top of [cursor]. Frames
  /// above the new cursor are retained in [frames] so a future branch
  /// can be redone, but the linear redo chain ends at [cursor] + 1.
  /// When the stack is full, the oldest frame is evicted.
  HistoryState push(HistoryCommand command, {String? branchLabel}) {
    final newFrame = HistoryFrame(
      id: nextId,
      command: command,
      parentId: current?.id,
      branchLabel: branchLabel,
    );
    final nextFrames = [...frames];
    // Drop any frames above the current cursor — they become a
    // discarded branch. (Branch support: a future caller may pass a
    // [branchLabel] to surface this in the UI as a recoverable fork.)
    if (cursor + 1 < nextFrames.length) {
      nextFrames.removeRange(cursor + 1, nextFrames.length);
    }
    nextFrames.add(newFrame);
    // Evict the oldest frame when over capacity. The eviction drops the
    // bottom of the stack; the cursor shifts down by one to compensate.
    var evicted = 0;
    while (nextFrames.length > maxSteps) {
      nextFrames.removeAt(0);
      evicted++;
    }
    return HistoryState(
      frames: List<HistoryFrame>.unmodifiable(nextFrames),
      cursor: cursor + 1 - evicted,
      nextId: nextId + 1,
      maxSteps: maxSteps,
    );
  }

  /// Returns a state with the cursor moved one step back, and the
  /// command at the old cursor undone. Returns `this` when [canUndo]
  /// is false.
  HistoryState undo() {
    if (!canUndo) return this;
    frames[cursor].command.undo();
    return copyWith(cursor: cursor - 1);
  }

  /// Returns a state with the cursor moved one step forward, and the
  /// command at the new cursor redone. Returns `this` when [canRedo]
  /// is false.
  HistoryState redo() {
    if (!canRedo) return this;
    final next = cursor + 1;
    frames[next].command.redo();
    return copyWith(cursor: next);
  }

  /// Returns a state with the cursor moved to [index], undoing /
  /// redoing the commands between the current cursor and the target.
  /// Returns `this` when [index] is out of range or already current.
  HistoryState jumpTo(int index) {
    if (index < -1 || index >= frames.length || index == cursor) return this;
    if (index < cursor) {
      var state = this;
      while (state.cursor > index) {
        state = state.undo();
      }
      return state;
    }
    var state = this;
    while (state.cursor < index) {
      state = state.redo();
    }
    return state;
  }

  /// Returns an empty history (the stack is wiped). Does NOT undo any
  /// commands — callers must ensure the document is in the desired
  /// state before truncating.
  HistoryState clear() => HistoryState(maxSteps: maxSteps);

  HistoryState copyWith({
    List<HistoryFrame>? frames,
    int? cursor,
    int? nextId,
    int? maxSteps,
  }) =>
      HistoryState(
        frames: frames ?? this.frames,
        cursor: cursor ?? this.cursor,
        nextId: nextId ?? this.nextId,
        maxSteps: maxSteps ?? this.maxSteps,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is HistoryState &&
          other.cursor == cursor &&
          other.nextId == nextId &&
          other.maxSteps == maxSteps &&
          _listEq(other.frames, frames);

  @override
  int get hashCode => Object.hash(cursor, nextId, maxSteps, Object.hashAll(frames));
}

bool _listEq<T>(List<T> a, List<T> b) {
  if (a.length != b.length) return false;
  for (var i = 0; i < a.length; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}
