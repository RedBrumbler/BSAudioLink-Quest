#include "Providers/GameProvider.hpp"
#include "logging.hpp"

DEFINE_TYPE(AudioLink, GameProvider);

namespace AudioLink {

    void GameProvider::ctor(AudioLink* audioLink, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ColorScheme* colorScheme) {
        static auto _audioLink_info = il2cpp_functions::class_get_field_from_name(klass, "_audioLink");
        static auto _audioTimeSyncController_info = il2cpp_functions::class_get_field_from_name(klass, "_audioTimeSyncController");
        static auto _colorScheme_info = il2cpp_functions::class_get_field_from_name(klass, "_colorScheme");

        il2cpp_functions::field_set_value_object(this, _audioLink_info, audioLink);
        il2cpp_functions::field_set_value_object(this, _audioTimeSyncController_info, audioTimeSyncController);
        il2cpp_functions::field_set_value_object(this, _colorScheme_info, colorScheme);
    }

    void GameProvider::Initialize() {
        INFO("GameProvider Initialize");
        _audioLink->SetAudioSource(_audioTimeSyncController->_audioSource);
        _audioLink->SetColorScheme(_colorScheme);
    }
}
