# v0.55-C: ProGuard / R8 keep rules for the Feather-Krita Android release build.
#
# These rules supplement Flutter's default consumer-proguard-rules (auto-applied
# by the Flutter Gradle plugin) which already keep:
#   * io.flutter.** (Flutter engine JNI surface)
#   * io.flutter.plugins.** (FlutterFire / path_provider / etc.)
#
# Local keeps below protect the Feather-Krita-specific reflective JNI surface
# the native krita_bridge .so depends on at runtime.

# ---------------------------------------------------------------------------
# 1. MainActivity + its companion (static) initializer.
#
# MainActivity.<clinit>() calls System.loadLibrary("krita_bridge") which is
# the ONLY place the native lib is dlopen'd on Android. If R8 strips the
# companion init (or renames MainActivity so the AndroidManifest launcher
# can't find it), the .so never loads and every FFI lookup in
# lib/engine/krita_bridge/krita_bindings.dart returns a MissingPluginException.
# Flutter's default rules keep the launcher activity, but we keep it
# explicitly here for safety + clarity.
# ---------------------------------------------------------------------------
-keep class com.featherkrita.feather_krita.MainActivity { *; }
-keepclassmembers class com.featherkrita.feather_krita.MainActivity {
    public *;
    static *;
}

# ---------------------------------------------------------------------------
# 2. Qt5 stub Java classes (org.qtproject.qt5.android.*).
#
# The merged krita_bridge .so's JNI_OnLoad (Qt5Core's) reflectively FindClass
# calls these stubs (QtNative, QtApplication). Without them, JNI_OnLoad fails
# and Qt's g_javaVm pointer is left null — every QJNIEnvironmentPrivate path
# crashes (fault 0x0 in krita_brush_set_size → KLocalizedString::toString()).
# The .so stays loaded (dlopen succeeds) so FFI symbol lookups still work,
# but the brush-engine path crashes. Keep the stubs.
# ---------------------------------------------------------------------------
-keep class org.qtproject.qt5.android.** { *; }

# ---------------------------------------------------------------------------
# 3. Native method declarations (defensive — none today, but if a future
# PR adds @JvmStatic external fun to MainActivity or a plugin, R8 must not
# rename the Java side or the JNI RegisterNatives call won't match).
# ---------------------------------------------------------------------------
-keepclasseswithmembernames class * {
    native <methods>;
}
