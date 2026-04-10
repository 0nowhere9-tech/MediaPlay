cmake_minimum_required(VERSION 3.16)
project(MediaPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Concurrent Core Quick Multimedia Network QuickControls2)

qt_standard_project_setup()

# Silence Qt 6.10 policy warnings
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(MediaPlayer
    src/main.cpp
    src/VideoModel.cpp
    src/VideoModel.h
    src/VideoItem.cpp
    src/VideoItem.h
    src/JsonLoader.cpp
    src/JsonLoader.h
    src/ExtractionManager.cpp
    src/ExtractionManager.h
    providers/BaseProvider.h
    providers/HentaiCityProvider.cpp
    providers/HentaiCityProvider.h
    providers/ProviderRegistry.cpp
    providers/ProviderRegistry.h
    extractors/BaseExtractor.h
    extractors/HentaiCityExtractor.h
    extractors/HentaiCityExtractor.cpp
    extractors/ExtractorRegistry.h
    extractors/ExtractorRegistry.cpp
)

qt_add_qml_module(MediaPlayer
    URI mediaplayer
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/VideosPage.qml
        qml/VideoCard.qml
        qml/VideoPlayer.qml
        qml/PlayerControls.qml
        qml/TopBar.qml
        qml/SideBar.qml
        qml/NavItem.qml
        qml/StatChip.qml
        qml/MetaStat.qml
        qml/MetaField.qml
        qml/SourcesPage.qml
        qml/SourceRow.qml
        qml/SettingsStore.qml
        qml/SettingsPage.qml
        qml/SettingsSection.qml
        qml/SettingSlider.qml
    RESOURCES
        # Resource prefix will be :/qt/qml/mediaplayer/resources/sample/all_videos.json
        # Access via "qrc:/qt/qml/mediaplayer/resources/sample/all_videos.json"
        resources/sample/all_videos.json
)

target_link_libraries(MediaPlayer PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Quick
    Qt6::Multimedia
    Qt6::Network
    Qt6::QuickControls2
)

target_include_directories(MediaPlayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/providers
    ${CMAKE_CURRENT_SOURCE_DIR}/extractors
)

install(TARGETS MediaPlayer DESTINATION .)
