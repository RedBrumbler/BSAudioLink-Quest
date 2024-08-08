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
#include "AssetBundleManager.hpp"

// based on https://github.com/Aeroluna/BSAudioLink/blob/master/AudioLink/Scripts/AudioLink.cs
DECLARE_CLASS_CODEGEN_INTERFACES(AudioLink, AudioLink, System::Object, std::vector<Il2CppClass*>({classof(Zenject::ITickable*), classof(Zenject::IInitializable*), classof(::System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, _customThemeColor0);
    DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, _customThemeColor1);
    DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, _customThemeColor2);
    DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, _customThemeColor3);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AudioSource*, _audioSource);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Material*, _audioMaterial);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, _testPlane);
    DECLARE_INSTANCE_FIELD_PRIVATE(AssetBundleManager*, _assetBundleManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _audioFramesL);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _audioFramesR);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _samples);

    DECLARE_INSTANCE_FIELD_PRIVATE(double, _elapsedTime);
    DECLARE_INSTANCE_FIELD_PRIVATE(double, _elapsedTimeMSW);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _networkTimeMS);
    DECLARE_INSTANCE_FIELD_PRIVATE(double, _networkTimeMSAccumulatedError);
    DECLARE_INSTANCE_FIELD_PRIVATE(double, _fPSTime);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _fPSCount);

    DECLARE_INSTANCE_FIELD_PRIVATE(int, _rightChannelTestCounter);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _ignoreRightChannel);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _initialized);

    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor0);
    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor1);
    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor2);
    DECLARE_INSTANCE_METHOD(Sombrero::FastColor, get_customThemeColor3);
    DECLARE_INSTANCE_METHOD(UnityEngine::AudioSource*, get_audioSource);
    DECLARE_INSTANCE_METHOD(UnityEngine::Material*, get_audioMaterial);
    DECLARE_INSTANCE_METHOD(ArrayW<float>, get_audioFramesL);
    DECLARE_INSTANCE_METHOD(ArrayW<float>, get_audioFramesR);
    DECLARE_INSTANCE_METHOD(ArrayW<float>, get_samples);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_INSTANCE_METHOD(void, UpdateSettings);
    DECLARE_INSTANCE_METHOD(void, UpdateThemeColors);
    DECLARE_INSTANCE_METHOD(void, FPSUpdate);
    DECLARE_INSTANCE_METHOD(void, SendAudioOutputData);
    DECLARE_INSTANCE_METHOD(void, SetAudioSource, UnityEngine::AudioSource* audioSource);
    DECLARE_INSTANCE_METHOD(void, SetColorScheme, GlobalNamespace::ColorScheme* colorScheme);

    public:
        DECLARE_CTOR(ctor, AssetBundleManager* assetBundleManager);
)
