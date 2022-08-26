#include "Providers/MenuProvider.hpp"

#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
DEFINE_TYPE(AudioLink, MenuProvider);

namespace AudioLink {
    MenuProvider* MenuProvider::instance;
    MenuProvider* MenuProvider::get_instance() {
        return instance;
    }
    
    void MenuProvider::ctor(AudioLink* audioLink, GlobalNamespace::PlayerDataModel* playerDataModel) {
        _audioLink = audioLink;
        _playerDataModel = playerDataModel;
        instance = this;
    }

    void MenuProvider::dtor() {
        instance = nullptr;
        Finalize();
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