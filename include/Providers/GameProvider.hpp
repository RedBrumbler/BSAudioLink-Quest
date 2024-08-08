#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "Zenject/IInitializable.hpp"

#include "AudioLink.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(AudioLink, GameProvider, System::Object, classof(Zenject::IInitializable*),
    DECLARE_INSTANCE_FIELD_PRIVATE(::AudioLink::AudioLink*, _audioLink);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ColorScheme*, _colorScheme);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
    public:
        DECLARE_CTOR(ctor, ::AudioLink::AudioLink* audioLink, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::ColorScheme* colorScheme);
)
