{
  description = "MediaPlayer — Qt6 C++/QML dev shell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        qt = pkgs.qt6;
      in
      {
        devShells.default = pkgs.mkShell {
          name = "mediaplayer";

          packages = with pkgs; [
            # ── Build toolchain ───────────────────────────────────────────
            cmake
            ninja
            gcc
            pkg-config

            # ── Qt6 modules ───────────────────────────────────────────────
            qt.qtbase          # Core, Gui, Quick, QML
            qt.qtdeclarative   # QML engine, Qt Quick, Qt Quick Controls 2
            qt.qtmultimedia    # MediaPlayer, VideoOutput, AudioOutput
            qt.qttools         # moc, rcc, qmake
            qt.wrapQtAppsHook

            # ── Runtime: audio (pipewire preferred, pulse fallback) ───────
            pipewire
            libpulseaudio

            # ── Runtime: video codec backend via GStreamer ─────────────────
            gst_all_1.gstreamer
            gst_all_1.gst-plugins-base
            gst_all_1.gst-plugins-good
            gst_all_1.gst-plugins-bad
            gst_all_1.gst-plugins-ugly
            gst_all_1.gst-libav

            # ── Runtime: graphics ─────────────────────────────────────────
            libGL
            libGLU
            libx11
            libxext
            libxrandr
            wayland

            # ── Dev extras ────────────────────────────────────────────────
            clang-tools
            gdb
          ];

          shellHook = ''
            echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
            echo " MediaPlayer dev shell (Qt ${qt.qtbase.version})"
            echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
            echo "  cmake -B build -S . -G Ninja"
            echo "  cmake --build build"
            echo "  ./build/MediaPlayer"
            echo ""

            # GStreamer codec plugins
            export GST_PLUGIN_PATH="${pkgs.gst_all_1.gst-plugins-base}/lib/gstreamer-1.0:${pkgs.gst_all_1.gst-plugins-good}/lib/gstreamer-1.0:${pkgs.gst_all_1.gst-plugins-bad}/lib/gstreamer-1.0:${pkgs.gst_all_1.gst-plugins-ugly}/lib/gstreamer-1.0:${pkgs.gst_all_1.gst-libav}/lib/gstreamer-1.0"

            # Qt plugins
            export QT_PLUGIN_PATH="${qt.qtbase}/${qt.qtbase.qtPluginPrefix}:${qt.qtmultimedia}/${qt.qtbase.qtPluginPrefix}"

            # Explicitly scope QML imports to only our Qt — prevents KDE/system
            # QML overlays (e.g. org.kde.breeze) from hijacking Qt built-ins
            export QML2_IMPORT_PATH="${qt.qtdeclarative}/${qt.qtbase.qtQmlPrefix}:${qt.qtmultimedia}/${qt.qtbase.qtQmlPrefix}"
            export QT_QML_IMPORT_PATH="$QML2_IMPORT_PATH"

            # Pipewire library path so QtMultimedia can dlopen it
            export LD_LIBRARY_PATH="${pkgs.pipewire}/lib:${pkgs.libpulseaudio}/lib:$LD_LIBRARY_PATH"

            # Prefer Wayland; fall back to xcb
            export QT_QPA_PLATFORM="wayland;xcb"
          '';
        };
      }
    );
}
