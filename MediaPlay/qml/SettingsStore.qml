pragma Singleton
import QtQuick

// SettingsStore — single source of truth for all app settings.
//
// On startup, each property reads its persisted value from settingsManager
// (the C++ context property) with a sensible default as fallback.
//
// Whenever a property changes, it is staged in settingsManager via set().
// The actual write to resources/settings.json happens once on app quit
// (QGuiApplication::aboutToQuit → SettingsManager::save), so disk I/O
// only occurs once per session rather than on every slider move.
//
// To add a new setting:
//   1. Add a property here, initialised via settingsManager.get()
//   2. Add an onChanged handler that calls settingsManager.set()
//   3. Add the matching SettingSlider (or other control) in SettingsPage.qml

QtObject {
    id: root

    // ── Videos ───────────────────────────────────────────────────
    property real videosScrollSpeed: settingsManager.get("videos/scrollSpeed", 3.0)
    onVideosScrollSpeedChanged: settingsManager.set("videos/scrollSpeed", videosScrollSpeed)

    property bool videosInfiniteScroll: settingsManager.get("videos/infiniteScroll", false)
    onVideosInfiniteScrollChanged: settingsManager.set("videos/infiniteScroll", videosInfiniteScroll)

    // ── Future setting groups go here ─────────────────────────────
    // property bool playerAutoplay: settingsManager.get("player/autoplay", true)
    // onPlayerAutoplayChanged: settingsManager.set("player/autoplay", playerAutoplay)
}
