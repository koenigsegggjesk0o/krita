// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// apple_pencil_handler_test.dart — verifies the v0.54-D Apple Pencil
// wiring: the [ApplePencilHandler] emits the right [PencilAction]s for
// the host's config, the [InputState] mirrors stylus samples, the
// [ApplePencilChannel] platform-gates correctly, and the
// [ApplePencilScope] InheritedWidget is discoverable by descendants.
//
// These tests run on the host platform (not iOS), so they exercise the
// pure-Dart handler + the non-iOS channel stub. The live iOS EventChannel
// path (native `UIPencilInteractionDelegate` → `feather_krita/apple_pencil`
// sink) requires a real iOS target and is out of scope for `flutter test`
// — see apple_pencil_channel.dart's header for the native-side gap.

import 'dart:async';

import 'package:feather_krita/core/interaction/apple_pencil_channel.dart';
import 'package:feather_krita/core/interaction/apple_pencil_handler.dart';
import 'package:feather_krita/core/interaction/input_state.dart';
import 'package:feather_krita/core/math/math.dart';
import 'package:flutter/widgets.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('ApplePencilHandler — action emission (host config)', () {
    // The host (main_screen.dart) wires the handler with this config:
    // squeeze = squeezeMenu (emits SqueezeAction), doubleTap = injector
    // (emits DoubleTapAction). This keeps the handler a pure event source
    // and lets the host own the side-effects (pen↔eraser flip, injector
    // toggle). These tests pin that contract.
    const config = PencilGestureConfig(
      squeeze: PencilGestureBinding.squeezeMenu,
      doubleTap: PencilGestureBinding.injector,
    );

    test('double-tap emits a DoubleTapAction', () async {
      final handler = ApplePencilHandler(InputState(), config: config);
      final actions = <PencilAction>[];
      final sub = handler.actions.listen(actions.add);

      handler.onDoubleTap();
      // Broadcast streams are async; let the listener drain.
      await Future<void>.delayed(Duration.zero);

      expect(actions, hasLength(1));
      expect(actions.single, isA<DoubleTapAction>());

      await sub.cancel();
      handler.dispose();
    });

    test('squeeze emits a SqueezeAction carrying the position', () async {
      final handler = ApplePencilHandler(InputState(), config: config);
      final actions = <PencilAction>[];
      final sub = handler.actions.listen(actions.add);

      handler.onSqueeze(position: Vector2(120.0, 240.0));
      await Future<void>.delayed(Duration.zero);

      expect(actions, hasLength(1));
      final a = actions.single as SqueezeAction;
      expect(a.position, isNotNull);
      expect(a.position!.x, 120.0);
      expect(a.position!.y, 240.0);

      await sub.cancel();
      handler.dispose();
    });

    test('squeeze with no position emits a SqueezeAction with null position',
        () async {
      final handler = ApplePencilHandler(InputState(), config: config);
      final actions = <PencilAction>[];
      final sub = handler.actions.listen(actions.add);

      handler.onSqueeze();
      await Future<void>.delayed(Duration.zero);

      expect(actions.single, isA<SqueezeAction>());
      expect((actions.single as SqueezeAction).position, isNull);

      await sub.cancel();
      handler.dispose();
    });

    test('squeeze release emits a SqueezeReleaseAction (after a squeeze)',
        () async {
      final handler = ApplePencilHandler(InputState(), config: config);
      final actions = <PencilAction>[];
      final sub = handler.actions.listen(actions.add);

      handler.onSqueeze();
      handler.onSqueezeRelease();
      await Future<void>.delayed(Duration.zero);

      // squeeze → SqueezeAction, release → SqueezeReleaseAction.
      expect(actions, hasLength(2));
      expect(actions[0], isA<SqueezeAction>());
      expect(actions[1], isA<SqueezeReleaseAction>());

      await sub.cancel();
      handler.dispose();
    });
  });

  group('ApplePencilHandler — stylus sample → InputState mirror', () {
    test('onStylusSample pushes clamped pressure + tilt into InputState', () {
      final input = InputState();
      final handler = ApplePencilHandler(input);

      handler.onStylusSample(
        pressure: 1.5, // out-of-range high → clamped to 1.0
        tilt: Vector2(0.1, 0.2),
        azimuth: 0.3,
        altitude: 0.4,
        twist: 0.5,
      );

      expect(input.stylus.pressure, 1.0, reason: 'pressure must be clamped to [0,1]');
      expect(input.stylus.tilt.x, 0.1);
      expect(input.stylus.tilt.y, 0.2);
      expect(input.stylus.azimuthRadians, 0.3);
      expect(input.stylus.altitudeRadians, 0.4);
      expect(input.stylus.twistRadians, 0.5);
      expect(input.lastDevice, InputDevice.stylus,
          reason: 'a stylus sample marks the device as stylus');

      handler.dispose();
    });

    test('onStylusSample clamps negative pressure to 0', () {
      final input = InputState();
      final handler = ApplePencilHandler(input);

      handler.onStylusSample(pressure: -0.4);

      expect(input.stylus.pressure, 0.0);
      handler.dispose();
    });

    test('onStylusUp resets the stylus to idle (pressure 0) and ends squeeze-drag',
        () {
      final input = InputState();
      final handler = ApplePencilHandler(input);

      handler.onStylusSample(pressure: 0.8, tilt: Vector2(0.1, 0.2));
      expect(input.stylus.pressure, 0.8);

      handler.onStylusUp();
      expect(input.stylus.pressure, 0.0, reason: 'stylus up → idle sample');
      expect(input.stylus.tilt.x, 0.0);
      expect(input.stylus.tilt.y, 0.0);
      expect(handler.isSqueezing, isFalse,
          reason: 'stylus up ends any in-flight squeeze-drag');

      handler.dispose();
    });
  });

  group('ApplePencilChannel — platform gating', () {
    test('wire() returns an inactive stub on non-iOS test hosts', () {
      // flutter_test runs on the host (Linux/macOS/Windows), never iOS, so
      // isApplePencilPlatform is false here. The channel must be inert —
      // no EventChannel subscription, active == false — so the host's
      // stock gesture flow is untouched on every non-iOS platform.
      expect(isApplePencilPlatform, isFalse,
          reason: 'test host is not iOS');
      final handler = ApplePencilHandler(InputState());
      final channel = ApplePencilChannel.wire(handler);
      expect(channel.active, isFalse,
          reason: 'non-iOS wire() must return a disabled stub');
      handler.dispose();
      channel.dispose();
    });
  });

  group('ApplePencilScope — InheritedWidget lookup', () {
    testWidgets('maybeOf returns null when no scope is mounted above the context',
        (tester) async {
      late BuildContext captured;
      await tester.pumpWidget(
        Builder(
          builder: (context) {
            captured = context;
            return const SizedBox.shrink();
          },
        ),
      );
      expect(ApplePencilScope.maybeOf(captured), isNull,
          reason: 'no ApplePencilScope ancestor → null');
    });

    testWidgets('maybeOf returns the handler when a scope is mounted',
        (tester) async {
      final handler = ApplePencilHandler(InputState());
      late BuildContext captured;
      await tester.pumpWidget(
        ApplePencilScope(
          handler: handler,
          child: Builder(
            builder: (context) {
              captured = context;
              return const SizedBox.shrink();
            },
          ),
        ),
      );
      expect(ApplePencilScope.maybeOf(captured), same(handler),
          reason: 'descendant context must resolve the mounted handler');
      handler.dispose();
    });

    test('updateShouldNotify is false for the same handler instance', () {
      final handler = ApplePencilHandler(InputState());
      final a = ApplePencilScope(handler: handler, child: const SizedBox.shrink());
      final b = ApplePencilScope(handler: handler, child: const SizedBox.shrink());
      expect(a.updateShouldNotify(b), isFalse,
          reason: 'identical handler → no rebuild needed');
      final other = ApplePencilHandler(InputState());
      final c = ApplePencilScope(handler: other, child: const SizedBox.shrink());
      expect(a.updateShouldNotify(c), isTrue,
          reason: 'different handler → descendants must rebuild');
      handler.dispose();
      other.dispose();
    });
  });

  group('ApplePencilHandler — host wiring contract (main_screen config)', () {
    // Pins the exact config main_screen.dart uses so a future edit that
    // accidentally flips the bindings (e.g. makes double-tap call
    // flipToolPair internally instead of emitting an action) is caught.
    test('the host config emits actions for BOTH gestures (no internal tool flip)',
        () async {
      const config = PencilGestureConfig(
        squeeze: PencilGestureBinding.squeezeMenu,
        doubleTap: PencilGestureBinding.injector,
      );
      final input = InputState();
      final handler = ApplePencilHandler(input, config: config);
      final actions = <PencilAction>[];
      final sub = handler.actions.listen(actions.add);
      final toolBefore = input.activeTool;

      handler.onDoubleTap();
      handler.onSqueeze(position: Vector2(10, 20));
      await Future<void>.delayed(Duration.zero);

      // Both gestures emit actions...
      expect(actions.whereType<DoubleTapAction>(), hasLength(1));
      expect(actions.whereType<SqueezeAction>(), hasLength(1));
      // ...and NEITHER mutates the InputState's tool (the host owns the
      // side-effect via _setUiStateTool / _onUiStateChanged). This is the
      // contract that lets the host flip pen↔eraser (not draw↔drawShape)
      // on double-tap — the handler's own flipToolPair would flip the
      // wrong pair.
      expect(input.activeTool, toolBefore,
          reason: 'host config must not auto-flip the tool');

      await sub.cancel();
      handler.dispose();
    });
  });
}
