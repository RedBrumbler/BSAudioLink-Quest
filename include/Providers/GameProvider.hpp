#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "Zenject/IInitializable.hpp"

#include "AudioLink.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(AudioLink, GameProvider, Il2CppObject, classof(Zenject::IInitializable*),
    DECLARE_PRIVATE_FIELD(::AudioLink::AudioLink*, _audioLink);
    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
    DECLARE_PRIVATE_FIELD(GlobalNamespace::ColorScheme*, _colorScheme);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Zenject::IInitializable::Initialize>::get());
    public:
        DECLARE_CTOR(ctor, ::AudioLink::AudioLink* audioLink, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ColorScheme* colorScheme);
)
