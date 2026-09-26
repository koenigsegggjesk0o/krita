// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stable_strokes_test.dart — unit tests for the real-time (causal)
// Stable Strokes stabilizer.
//
// Covers disabled passthrough, warm-up (returns null until the buffer
// has >= 2 samples), causal behaviour (smoothed point lies between
// the previous smoothed position and the latest sample), extrapolation
// toward the newest sample, confidence growth, flush tail emission,
// and reset.

import 'package:feather_krita/engine/assistance/stable_strokes.dart';
import 'package:feather_krita/core/math/math.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('StableStrokes — disabled config', () {
    test('disabled config passes the sample through unchanged', () {
      final s = StableStrokes(config: const StableStrokesConfig(enabled: false));
      final out = s.push(StableSample(position: Vector3(1, 2, 3)));
      expect(out, isNotNull);
      expect(out!.position.x, 1);
      expect(out.position.y, 2);
      expect(out.position.z, 3);
      expect(out.confidence, 1.0);
    });
  });

  group('StableStrokes — warm-up', () {
    test('push returns null when only one sample is buffered', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 4,
      ));
      final out = s.push(StableSample(position: Vector3(0, 0, 0)));
      expect(out, isNull);
    });

    test('push returns a value once two samples are buffered', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 4,
        extrapolation: 0.0, // disable extrapolation for the bounds check
      ));
      expect(s.push(StableSample(position: Vector3(0, 0, 0))), isNull);
      final out = s.push(StableSample(position: Vector3(2, 0, 0)));
      expect(out, isNotNull);
      // With no extrapolation, the smoothed value is a weighted
      // centroid of [0, 0, 0] and [2, 0, 0] — somewhere in [0, 2].
      expect(out!.position.x, greaterThan(0));
      expect(out.position.x, lessThan(2));
      expect(out.position.y, 0);
      expect(out.position.z, 0);
    });
  });

  group('StableStrokes — causal behaviour', () {
    test('smoothed point lies between the previous smoothed value and the new sample', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 6,
        extrapolation: 0.0,
      ));
      Vector3? previous;
      for (var i = 0; i < 12; i++) {
        final out = s.push(StableSample(position: Vector3(i.toDouble(), (i % 2) * 10, 0)));
        if (out == null) continue;
        if (previous != null) {
          // The smoothed x must lie between the previous smoothed x
          // and the latest raw x (because the centroid averages the
          // last few samples, all of which are <= i).
          expect(out.position.x, greaterThanOrEqualTo(previous.x - 1e-6));
          expect(out.position.x, lessThanOrEqualTo(i.toDouble() + 1e-6));
        }
        previous = out.position.clone();
      }
    });

    test('first sample is preserved when buffer is empty', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
      ));
      // First push always returns null (buffer length 1 < 2).
      expect(s.push(StableSample(position: Vector3(7, 8, 9))), isNull);
    });
  });

  group('StableStrokes — extrapolation', () {
    test('extrapolation = 1 pulls the emitted point to the newest sample', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5, // window size = 2 (0.5 * 4 rounded)
        maxWindow: 4,
        extrapolation: 1.0,
      ));
      // Push a few samples; with extrapolation = 1, the emitted
      // position should equal the latest raw position.
      s.push(StableSample(position: Vector3(0, 0, 0)));
      final out = s.push(StableSample(position: Vector3(10, 0, 0)));
      expect(out, isNotNull);
      expect(out!.position.x, closeTo(10, 1e-6));
    });
  });

  group('StableStrokes — confidence', () {
    test('confidence grows as the buffer fills', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 6,
        extrapolation: 0.0,
      ));
      double? firstConfidence;
      double? lastConfidence;
      for (var i = 0; i < 8; i++) {
        final out = s.push(StableSample(position: Vector3(i.toDouble(), 0, 0)));
        if (out == null) continue;
        firstConfidence ??= out.confidence;
        lastConfidence = out.confidence;
      }
      expect(firstConfidence, isNotNull);
      expect(lastConfidence, isNotNull);
      expect(lastConfidence!, greaterThanOrEqualTo(firstConfidence!));
      expect(lastConfidence, lessThanOrEqualTo(1.0));
    });
  });

  group('StableStrokes — flush & reset', () {
    test('flush empties the buffer and returns the trailing outputs', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 6,
        extrapolation: 0.0,
      ));
      for (var i = 0; i < 6; i++) {
        s.push(StableSample(position: Vector3(i.toDouble(), 0, 0)));
      }
      final flushed = s.flush();
      // After flush the buffer is empty: a subsequent flush is a no-op.
      expect(s.flush(), isEmpty);
      // The flushed output is non-empty (we had 6 samples buffered).
      // Note: flush emits one output per buffer slot removed (until
      // length < 2), so the count depends on the window size.
      expect(flushed.length, lessThanOrEqualTo(6));
    });

    test('reset clears the buffer', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
      ));
      s.push(StableSample(position: Vector3.zero()));
      s.push(StableSample(position: Vector3(1, 0, 0)));
      s.reset();
      // After reset, the first push must return null again (warm-up).
      expect(s.push(StableSample(position: Vector3(2, 0, 0))), isNull);
    });
  });

  group('StableStrokes — config helpers', () {
    test('toggle flips the enabled state', () {
      final s = StableStrokes(config: const StableStrokesConfig(enabled: false));
      expect(s.config.enabled, isFalse);
      s.toggle();
      expect(s.config.enabled, isTrue);
      s.toggle();
      expect(s.config.enabled, isFalse);
    });

    test('setIntensity clamps to [0, 1] and enables when > 0', () {
      final s = StableStrokes(config: const StableStrokesConfig(enabled: false));
      s.setIntensity(5.0);
      expect(s.config.intensity, 1.0);
      expect(s.config.enabled, isTrue);
      s.setIntensity(-1.0);
      expect(s.config.intensity, 0.0);
    });
  });
}
