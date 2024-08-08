#include "Providers/MenuProvider.hpp"
#include "logging.hpp"

#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
DEFINE_TYPE(AudioLink, MenuProvider);

namespace AudioLink {
    MenuProvider* MenuProvider::instance;
    MenuProvider* MenuProvider::get_instance() {
        return instance;
    }

    void MenuProvider::ctor(AudioLink* audioLink, GlobalNamespace::PlayerDataModel* playerDataModel) {
        INFO("MenuProvider ctor!");
        static auto _audioLink_info = il2cpp_functions::class_get_field_from_name(klass, "_audioLink");
        static auto _playerDataModel_info = il2cpp_functions::class_get_field_from_name(klass, "_playerDataModel");

        il2cpp_functions::field_set_value_object(this, _audioLink_info, audioLink);
        il2cpp_functions::field_set_value_object(this, _playerDataModel_info, playerDataModel);
        instance = this;
    }

    void MenuProvider::dtor() {
        Finalize();
        instance = nullptr;
    }

    void MenuProvider::SongPreviewPlayerProvide(int activeChannel, ArrayW<GlobalNamespace::SongPreviewPlayer::AudioSourceVolumeController*> audioSourceControllers) {
        auto controller = audioSourceControllers[activeChannel];
        _audioLink->SetAudioSource(controller->audioSource);
    }

    void MenuProvider::ColorManagerInstallerProvide(GlobalNamespace::ColorSchemeSO* menuColorScheme) {
        auto overrideColorScheme = _playerDataModel->playerData->colorSchemesSettings->GetOverrideColorScheme();
        _audioLink->SetColorScheme(overrideColorScheme ? overrideColorScheme : menuColorScheme->get_colorScheme());
    }
}
