pragma Singleton
import QtQuick
import QtCore

// SettingsStore — singleton holding all user-configurable settings.
//
// Add new settings here as properties. They are automatically persisted
// via Qt's Settings. To add a setting for a new page:
//   1. Add a group object and property here
//   2. Bind Settings { ... } entries for persistence
//   3. Reference SettingsStore.videos.scrollSpeed etc. in your QML

QtObject {
    id: root

    // ── Videos page settings ──────────────────────────────────────
    readonly property QtObject videos: QtObject {
        // Pixels scrolled per mouse wheel tick (20–400)
        property real scrollSpeed: 3.0
    }

    // ── Persistence ───────────────────────────────────────────────
    // Add new Settings blocks here as new pages/sections are introduced.
    Settings {
        category: "videos"
        property alias scrollSpeed: root.videos.scrollSpeed
    }
}
