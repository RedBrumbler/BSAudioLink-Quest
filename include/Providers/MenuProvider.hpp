#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/ColorSchemeSO.hpp"
#include "AudioLink.hpp"

DECLARE_CLASS_CODEGEN(AudioLink, MenuProvider, System::Object,
    DECLARE_INSTANCE_FIELD_PRIVATE(::AudioLink::AudioLink*, _audioLink);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::PlayerDataModel*, _playerDataModel);

    DECLARE_INSTANCE_METHOD(void, SongPreviewPlayerProvide, int activeChannel, ArrayW<GlobalNamespace::SongPreviewPlayer::AudioSourceVolumeController*> audioSourceControllers);
    DECLARE_INSTANCE_METHOD(void, ColorManagerInstallerProvide, GlobalNamespace::ColorSchemeSO* menuColorScheme);
    public:
        static MenuProvider* get_instance();
        DECLARE_CTOR(ctor, AudioLink* audioLink, GlobalNamespace::PlayerDataModel* playerDataModel);
        DECLARE_DTOR(dtor);
    private:
        static MenuProvider* instance;
)
