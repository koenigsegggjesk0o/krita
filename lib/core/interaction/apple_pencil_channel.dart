// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// apple_pencil_channel.dart — Flutter ↔ native iOS bridge for Apple Pencil
// hardware gestures.
//
// The Apple Pencil's hardware double-tap (Pencil 2 / Pro) and squeeze
// (Pencil Pro) gestures are NOT detectable from Flutter's [PointerEvent]
// stream — they are delivered by iOS through `UIPencilInteraction` /
// `UIPencilHoverPreview` delegate callbacks on the native side. This file
// owns the Flutter half of that bridge: an [EventChannel] named
// `feather_krita/apple_pencil` that the native iOS runner feeds with
// decoded gesture events, and a thin decoder that forwards them into the
// pure-Dart [ApplePencilHandler].
//
// ── Platform gating ─────────────────────────────────────────────────────
// The channel is only useful on iOS/iPadOS. On every other platform
// (Android, Windows, Linux, macOS desktop, web) [ApplePencilChannel.wire]
// returns a disabled stub whose [active] is `false` and which performs no
// EventChannel subscription. This guarantees the wiring cannot break the
// non-iOS builds (the host checks [active] before subscribing).
//
// ── Native-side gap (HONEST) ─────────────────────────────────────────────
// As of v0.54-D the Flutter side is fully wired: the channel name is
// reserved, the event decoder is implemented, and the host subscribes on
// iOS. The matching native iOS code (`ios/Runner/ApplePencilDelegate.swift`
// implementing `UIPencilInteractionDelegate` + the EventChannel sink) is
// NOT in the repository — the project has no `ios/` folder yet. Until a
// macOS/iOS toolchain adds the native delegate, the channel receives no
// events and the hardware double-tap / squeeze gestures do not fire in
// the running app. Tilt IS live on iOS without native code (it is fed
// from Flutter's own stylus [PointerEvent] stream in `canvas_viewport.dart`).
//
// Event wire format (produced by the future native delegate):
//   Each event is a `Map<String, dynamic>` with a `kind` field:
//     kind == 'doubleTap'                 → handler.onDoubleTap()
//     kind == 'squeeze'   + 'x','y'       → handler.onSqueeze(position)
//     kind == 'squeezeRelease'            → handler.onSqueezeRelease()
//     kind == 'stylus'    + 'pressure'    → handler.onStylusSample(...)
//                       (+ optional 'tiltX','tiltY','azimuth','altitude',
//                        'twist' in radians)
//
// Depends on: lib/core/interaction/apple_pencil_handler.dart,
//             lib/core/interaction/input_state.dart, lib/core/math/math.dart

import 'dart:async';
import 'dart:io' show Platform;

import 'package:flutter/foundation.dart'
    show ErrorDescription, FlutterError, FlutterErrorDetails, kIsWeb;
import 'package:flutter/services.dart' show EventChannel;

import 'apple_pencil_handler.dart';
import '../math/math.dart';

/// Whether the current platform can deliver Apple Pencil hardware events.
///
/// `kIsWeb` is checked first because `dart:io`'s [Platform] throws on the
/// web target; on every non-iOS native platform the answer is `false`.
bool get isApplePencilPlatform => !kIsWeb && Platform.isIOS;

/// The Flutter ↔ native iOS bridge for Apple Pencil hardware gestures.
///
/// Construct with [ApplePencilChannel.wire]; the returned instance is
/// [active] only on iOS/iPadOS. Call [dispose] from the host's
/// `dispose()` to release the EventChannel subscription.
class ApplePencilChannel {
  ApplePencilChannel._(this._handler, this._active);

  final ApplePencilHandler? _handler;
  final bool _active;
  StreamSubscription<dynamic>? _sub;

  /// `true` only when the channel is live on iOS and forwarding events
  /// into a real [ApplePencilHandler]. `false` on every other platform
  /// (the wiring is inert — no EventChannel subscription is created).
  bool get active => _active;

  /// Wire the native iOS event stream into [handler].
  ///
  /// On non-iOS platforms this returns a disabled stub ([active] == false)
  /// and performs no EventChannel subscription, so the host pays zero
  /// runtime cost and the stock gesture flow is untouched.
  static ApplePencilChannel wire(ApplePencilHandler handler) {
    if (!isApplePencilPlatform) {
      return ApplePencilChannel._(null, false);
    }
    final channel = ApplePencilChannel._(handler, true);
    channel._start();
    return channel;
  }

  void _start() {
    // EventChannel name reserved for the native iOS delegate
    // (`ios/Runner/ApplePencilDelegate.swift`). See the file header for
    // the native-side gap status — the channel is inert until that
    // delegate exists and feeds the sink.
    const eventChannel = EventChannel('feather_krita/apple_pencil');
    _sub = eventChannel.receiveBroadcastStream().listen(
      _onEvent,
      onError: (Object e, StackTrace s) {
        // Swallow native-side errors — a missing/broken delegate must not
        // crash the editor. The Flutter-side handler stays usable (tilt
        // still flows from PointerEvents on iOS).
        FlutterError.reportError(FlutterErrorDetails(
          exception: e,
          stack: s,
          library: 'feather_krita.core.interaction',
          context: ErrorDescription('ApplePencil EventChannel error'),
        ));
      },
    );
  }

  void _onEvent(dynamic raw) {
    final handler = _handler;
    if (handler == null) return;
    if (raw is! Map) return;
    final kind = raw['kind'];
    switch (kind) {
      case 'doubleTap':
        handler.onDoubleTap();
        break;
      case 'squeeze':
        final x = _asDouble(raw['x']);
        final y = _asDouble(raw['y']);
        handler.onSqueeze(
          position: (x != null && y != null)
              ? Vector2(x, y)
              : null,
        );
        break;
      case 'squeezeRelease':
        handler.onSqueezeRelease();
        break;
      case 'stylus':
        handler.onStylusSample(
          pressure: _asDouble(raw['pressure']) ?? 0.0,
          tilt: (_asDouble(raw['tiltX']) != null && _asDouble(raw['tiltY']) != null)
              ? Vector2(_asDouble(raw['tiltX'])!, _asDouble(raw['tiltY'])!)
              : null,
          azimuth: _asDouble(raw['azimuth']) ?? 0.0,
          altitude: _asDouble(raw['altitude']) ?? 0.0,
          twist: _asDouble(raw['twist']) ?? 0.0,
        );
        break;
      default:
        // Unknown event kind — ignore. Forward-compatible: new native
        // event kinds added later won't crash older Flutter builds.
        break;
    }
  }

  static double? _asDouble(dynamic v) {
    if (v == null) return null;
    if (v is double) return v;
    if (v is int) return v.toDouble();
    if (v is num) return v.toDouble();
    return null;
  }

  /// Release the EventChannel subscription. Safe to call on the disabled
  /// stub (no-op).
  Future<void> dispose() async {
    await _sub?.cancel();
    _sub = null;
  }
}
