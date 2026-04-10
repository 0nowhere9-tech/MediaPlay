# MediaPlayer

A modular C++/QML media player with a provider-based architecture for handling
videos from multiple sources.

---

## Architecture

```
mediaplayer/
├── src/
│   ├── main.cpp            — App entry point, QML engine setup
│   ├── VideoItem.h/.cpp    — Plain data class for a single video
│   ├── VideoModel.h/.cpp   — QAbstractListModel exposing items to QML
│   └── JsonLoader.h/.cpp   — Reads JSON → dispatches to providers
│
├── providers/
│   ├── BaseProvider.h          — Abstract interface all providers implement
│   ├── ProviderRegistry.h/.cpp — Singleton map: sourceName → provider
│   └── HentaiCityProvider.h/.cpp — Concrete provider for HentaiCity JSON
│
├── qml/
│   ├── Main.qml            — Root window, navigation state, page stack
│   ├── SideBar.qml         — Left nav panel
│   ├── TopBar.qml          — Top bar with page title and back button
│   ├── VideosPage.qml      — Scrollable grid of VideoCard items
│   ├── VideoCard.qml       — Single card: thumbnail, title, stats
│   ├── VideoPlayer.qml     — Full player: video output + metadata panel
│   └── PlayerControls.qml  — Seek bar, play/pause, volume
│
└── resources/
    └── sample/all_videos.json  — Bundled sample data
```

---

## Adding a New Video Source

1. Create `providers/MySourceProvider.h` subclassing `BaseProvider`.
2. Implement `sourceName()` (must match the `"source"` field in JSON) and
   `parseItem()` to map your JSON fields into `VideoItem`.
3. Register it in `ProviderRegistry::ProviderRegistry()`:
   ```cpp
   registerProvider(std::make_unique<MySourceProvider>());
   ```

That's it. The loader, model, and UI require no changes.

---

## Build

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/MediaPlayer
```

**Requirements:** Qt 6.4+ with `Qt6::Quick`, `Qt6::Multimedia`, `Qt6::Network`.

---

## Loading a Custom JSON File

From QML (or C++):
```qml
videoModel.loadFromFile("/path/to/your/videos.json")
```

The JSON must be an array of objects each with at minimum:
- `"source"` — must match a registered provider name
- `"videoUrl"` — playback URL
