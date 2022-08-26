#include "Providers/GameProvider.hpp"

DEFINE_TYPE(AudioLink, GameProvider);

namespace AudioLink {
    
    void GameProvider::ctor(AudioLink* audioLink, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ColorScheme* colorScheme) {
        _audioLink = audioLink;
        _audioTimeSyncController = audioTimeSyncController;
        _colorScheme = colorScheme;
    }

    void GameProvider::Initialize() {
        _audioLink->SetAudioSource(_audioTimeSyncController->audioSource);
        _audioLink->SetColorScheme(_colorScheme);
    }
}