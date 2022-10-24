#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "Zenject/ILateTickable.hpp"
#include "Zenject/ITickable.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/DiContainer.hpp"
#include "System/IDisposable.hpp"

#include "UnityEngine/Material.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/GameObject.hpp"

#include "GlobalNamespace/ColorScheme.hpp"

#include "sombrero/shared/FastColor.hpp"

#define INTERFACES {classof(Zenject::ITickable*), classof(Zenject::IInitializable*), classof(::System::IDisposable*)}

// based on https://github.com/Aeroluna/BSAudioLink/blob/master/AudioLink/Scripts/AudioLink.cs
___DECLARE_TYPE_WRAPPER_INHERITANCE(AudioLink, AudioLink, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "AudioLink", INTERFACES, 0, nullptr,
    DECLARE_PRIVATE_FIELD(Sombrero::FastColor, _customThemeColor0);
    DECLARE_PRIVATE_FIELD(Sombrero::FastColor, _customThemeColor1);
    DECLARE_PRIVATE_FIELD(Sombrero::FastColor, _customThemeColor2);
    DECLARE_PRIVATE_FIELD(Sombrero::FastColor, _customThemeColor3);
    DECLARE_PRIVATE_FIELD(UnityEngine::AudioSource*, _audioSource);
    DECLARE_PRIVATE_FIELD(UnityEngine::Material*, _audioMaterial);
    DECLARE_PRIVATE_FIELD(UnityEngine::GameObject*, _testPlane);

    DECLARE_PRIVATE_FIELD(ArrayW<float>, _audioFramesL);
    DECLARE_PRIVATE_FIELD(ArrayW<float>, _audioFramesR);
    DECLARE_PRIVATE_FIELD(ArrayW<float>, _samples);

    DECLARE_PRIVATE_FIELD(double, _elapsedTime);
    DECLARE_PRIVATE_FIELD(double, _elapsedTimeMSW);
    DECLARE_PRIVATE_FIELD(int, _networkTimeMS);
    DECLARE_PRIVATE_FIELD(double, _networkTimeMSAccumulatedError);
    DECLARE_PRIVATE_FIELD(double, _fPSTime);
    DECLARE_PRIVATE_FIELD(int, _fPSCount);

    DECLARE_PRIVATE_FIELD(int, _rightChannelTestCounter);
    DECLARE_PRIVATE_FIELD(bool, _ignoreRightChannel);
    DECLARE_PRIVATE_FIELD(bool, _initialized);

    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor0);
    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor1);
    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor2);
    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor3);
    DECLARE_INSTANCE_METHOD(UnityEngine::AudioSource*, get_audioSource);
    DECLARE_INSTANCE_METHOD(UnityEngine::Material*, get_audioMaterial);
    DECLARE_INSTANCE_METHOD(ArrayW<float>, get_audioFramesL);
    DECLARE_INSTANCE_METHOD(ArrayW<float>, get_audioFramesR);
    DECLARE_INSTANCE_METHOD(ArrayW<float>, get_samples);

    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_INSTANCE_METHOD(void, UpdateSettings);
    DECLARE_INSTANCE_METHOD(void, UpdateThemeColors);
    DECLARE_INSTANCE_METHOD(void, FPSUpdate);
    DECLARE_INSTANCE_METHOD(void, SendAudioOutputData);
    DECLARE_INSTANCE_METHOD(void, SetAudioSource, UnityEngine::AudioSource* audioSource);
    DECLARE_INSTANCE_METHOD(void, SetColorScheme, GlobalNamespace::ColorScheme* colorScheme);

    public:
        DECLARE_CTOR(ctor);
)