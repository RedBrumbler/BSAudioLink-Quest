#include "AudioLink.hpp"
#include "AssetBundleManager.hpp"

#include "config.hpp"
#include "ConfigProperties.hpp"
#include "ShaderProperties.hpp"

#include "UnityEngine/Time.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Rendering/RenderTextureSubElement.hpp"
#include <chrono>

#include "sombrero/shared/FastVector3.hpp"

DEFINE_TYPE(AudioLink, AudioLink);

using namespace UnityEngine;

void GetOutputDataHelper(UnityEngine::AudioSource* instance, ArrayW<float>& out, int channel) {
    using GetOutputDataHelperMethod = function_ptr_t<void, UnityEngine::AudioSource*, ArrayW<float>, int>;
    static auto getOutputDataHelper = reinterpret_cast<GetOutputDataHelperMethod>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::GetOutputDataHelper"));
    getOutputDataHelper(instance, out, channel);
}

inline float GetSpatialBlendMix(UnityEngine::AudioSource* self) {
    // this is what the icall in unity does, so this is what we will do
    return 0;
    using Get_SpatialBlendMix = function_ptr_t<float, UnityEngine::AudioSource*>;
    static auto get_SpatialBlendMix = reinterpret_cast<Get_SpatialBlendMix>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::GetSpatialBlendMix"));
    return get_SpatialBlendMix(self);

    // TODO: fix the icall because what we do now is just a no
    if (!self) return 0.0f;
    auto obj = *(long *)(self + 0x90);
    if (!obj) return 0.0f;
    return *(float*)(obj + 4);
}

extern Logger& getLogger();

namespace AudioLink {

    void AudioLink::ctor(AssetBundleManager* assetBundleManager) {
        getLogger().info("AudioLink ctor");

        _audioFramesL = ArrayW<float>(il2cpp_array_size_t(1023 * 4));
        _audioFramesR = ArrayW<float>(il2cpp_array_size_t(1023 * 4));
        _samples = ArrayW<float>(il2cpp_array_size_t(1023));

        _customThemeColor0 = Sombrero::FastColor::red();
        _customThemeColor1 = Sombrero::FastColor::cyan();
        _customThemeColor2 = Sombrero::FastColor::pink();
        _customThemeColor3 = Sombrero::FastColor::lightblue();

        _assetBundleManager = assetBundleManager;

    }

    void AudioLink::SetAudioSource(UnityEngine::AudioSource* audioSource) {
        getLogger().info("Set AudioSource: %p", audioSource);
        static auto _audioSource_info = il2cpp_functions::class_get_field_from_name(klass, "_audioSource");
        il2cpp_functions::field_set_value_object(this, _audioSource_info, audioSource);
    }

    void AudioLink::SetColorScheme(GlobalNamespace::ColorScheme* colorScheme) {
        if (!colorScheme) return;
        getLogger().info("Set ColorScheme: %p", colorScheme);
        _customThemeColor0 = colorScheme->get_environmentColor0();
        _customThemeColor1 = colorScheme->get_environmentColor1();
        _customThemeColor2 = colorScheme->get_environmentColor0Boost();
        _customThemeColor3 = colorScheme->get_environmentColor1Boost();

        UpdateThemeColors();
    }

    void AudioLink::Initialize() {
        getLogger().info("AudioLink Initialize");
        _assetBundleManager->Load();

        static auto _audioMaterial_info = il2cpp_functions::class_get_field_from_name(klass, "_audioMaterial");
        il2cpp_functions::field_set_value_object(this, _audioMaterial_info, _assetBundleManager->get_material());
        auto audioRenderTexture = _assetBundleManager->get_renderTexture();

        getLogger().info("SetGlobalRenderTexture");
        Shader::SetGlobalTexture(ShaderProperties::_audioTexture, audioRenderTexture, Rendering::RenderTextureSubElement::Default);
        _initialized = true;

        _testPlane = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Quad);
        _testPlane->get_transform()->set_localScale(Sombrero::FastVector3(2, 1, 1) * config.showTestPlane);
        _testPlane->get_transform()->set_localPosition({0, 0.1, 2});
        _testPlane->get_transform()->set_localEulerAngles({80, 0, 0});
        UnityEngine::Object::DontDestroyOnLoad(_testPlane);
        auto mat = _testPlane->GetComponent<UnityEngine::Renderer*>()->get_material();
        mat->set_shader(Shader::Find("Unlit/Texture"));
        mat->set_mainTexture(audioRenderTexture);
    }

    void AudioLink::Dispose() {
        if (_testPlane && _testPlane->m_CachedPtr.m_value)
            UnityEngine::Object::DestroyImmediate(_testPlane);
        _testPlane = nullptr;
    }

    void AudioLink::Tick() {
        if (!_initialized) return;
        _elapsedTime = Time::get_deltaTime();

        {
            double deltaTimeMS = Time::get_deltaTime() * 1000.0;
            int advanceTimeMS = (int)deltaTimeMS;
            _networkTimeMSAccumulatedError += deltaTimeMS - advanceTimeMS;
            if (_networkTimeMSAccumulatedError > 1)
            {
                _networkTimeMSAccumulatedError--;
                advanceTimeMS++;
            }

            _networkTimeMS += advanceTimeMS;
        }

        _fPSCount++;

        if (_elapsedTime >= _fPSTime) {
            FPSUpdate();
        }

        if (_audioMaterial && _audioMaterial->m_CachedPtr) {
            time_t rawtime = time(nullptr);
            auto timeInfo = localtime(&rawtime);
            unsigned long long timeOfDay = timeInfo->tm_hour * 60 * 60 + // hours since midnight * 60 * 60 for seconds since midnight
                                  timeInfo->tm_min * 60 + // minutes after hour * 60 for seconds since hour
                                  timeInfo->tm_sec; // seconds after minute
            // = current time in seconds in day

            _audioMaterial->SetVector(ShaderProperties::_advancedTimeProps, Vector4(
                _elapsedTime,
                _elapsedTimeMSW,
                timeOfDay,
                ConfigProperties::READBACK_TIME
            ));

            // Jan 1, 1970 = 62135596800 0000000.0 ticks.
            //double utcSecondsUnix = (DateTime.UtcNow.Ticks / 10 000 000.0) - 62135596800.0;
            // this works out to just being seconds since unix epoch
            double utcSecondsUnix = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            _audioMaterial->SetVector(ShaderProperties::_advancedTimeProps2, Vector4(
                _networkTimeMS & 65535,
                _networkTimeMS >> 16,
                std::floor(utcSecondsUnix / 86400),
                std::fmod(utcSecondsUnix, 86400)
            ));

            if (_audioSource && _audioSource->m_CachedPtr.m_value)
            {
                SendAudioOutputData();

                // Used to correct for the volume of the audio source component
                _audioMaterial->SetFloat(ShaderProperties::_sourceVolume, _audioSource->get_volume());
                _audioMaterial->SetFloat(ShaderProperties::_sourceSpatialBlend, GetSpatialBlendMix(_audioSource));
            }
        }
    }

    void AudioLink::UpdateSettings() {
        getLogger().info("Updating Settings");
        if (!_audioMaterial || !_audioMaterial->m_CachedPtr.m_value) return;
        _audioMaterial->SetFloat(ShaderProperties::_x0, ConfigProperties::X0);
        _audioMaterial->SetFloat(ShaderProperties::_x1, ConfigProperties::X1);
        _audioMaterial->SetFloat(ShaderProperties::_x2, ConfigProperties::X2);
        _audioMaterial->SetFloat(ShaderProperties::_x3, ConfigProperties::X3);
        _audioMaterial->SetFloat(ShaderProperties::_threshold0, ConfigProperties::THRESHOLD0);
        _audioMaterial->SetFloat(ShaderProperties::_threshold1, ConfigProperties::THRESHOLD1);
        _audioMaterial->SetFloat(ShaderProperties::_threshold2, ConfigProperties::THRESHOLD2);
        _audioMaterial->SetFloat(ShaderProperties::_threshold3, ConfigProperties::THRESHOLD3);
        _audioMaterial->SetFloat(ShaderProperties::_gain, ConfigProperties::GAIN);
        _audioMaterial->SetFloat(ShaderProperties::_fadeLength, ConfigProperties::FADE_LENGTH);
        _audioMaterial->SetFloat(ShaderProperties::_fadeExpFalloff, ConfigProperties::FADE_EXP_FALLOFF);
        _audioMaterial->SetFloat(ShaderProperties::_bass, ConfigProperties::BASS);
        _audioMaterial->SetFloat(ShaderProperties::_treble, ConfigProperties::TREBLE);
    }

    void AudioLink::UpdateThemeColors() {
        getLogger().info("Updating Color Scheme");
        if (!_audioMaterial || !_audioMaterial->m_CachedPtr.m_value) return;
        _audioMaterial->SetInt(ShaderProperties::_themeColorMode, ConfigProperties::THEME_COLOR_MODE);
        _audioMaterial->SetColor(ShaderProperties::_customThemeColor0ID, _customThemeColor0);
        _audioMaterial->SetColor(ShaderProperties::_customThemeColor1ID, _customThemeColor1);
        _audioMaterial->SetColor(ShaderProperties::_customThemeColor2ID, _customThemeColor2);
        _audioMaterial->SetColor(ShaderProperties::_customThemeColor3ID, _customThemeColor3);
    }

    void AudioLink::FPSUpdate() {
        if (_audioMaterial && _audioMaterial->m_CachedPtr.m_value) {
            _audioMaterial->SetVector(ShaderProperties::_versionNumberAndFPSProperty, Vector4(ConfigProperties::AUDIOLINK_VERSION_NUMBER, 0, _fPSCount, 1));
            _audioMaterial->SetVector(ShaderProperties::_playerCountAndData, {0, 0, 0, 0});
        } else {
            getLogger().error("_audioMaterial %p was not valid!\n", _audioMaterial);
            getLogger().error("Some properties have not been set...");
        }

        _fPSCount = 0;
        _fPSTime++;

        // Other things to handle every second.

        // This handles wrapping of the ElapsedTime so we don't lose precision
        // onthe floating point.
        const double ElapsedTimeMSWBoundary = 1024;
        if (_elapsedTime >= ElapsedTimeMSWBoundary)
        {
            // For particularly long running instances, i.e. several days, the first
            // few frames will be spent federating _elapsedTime into _elapsedTimeMSW.
            // This is fine.  It just means over time, the
            _fPSTime = 0;
            _elapsedTime -= ElapsedTimeMSWBoundary;
            _elapsedTimeMSW++;
        }

        // Finely adjust our network time estimate if needed.
        int networkTimeMSNow = (int)(Time::get_time() * 1000.0f);
        int networkTimeDelta = networkTimeMSNow - _networkTimeMS;
        if (networkTimeDelta > 3000 || networkTimeDelta < -3000) {
            // Major upset, reset.
            _networkTimeMS = networkTimeMSNow;
        } else {
            // Slowly correct the timebase.
            _networkTimeMS += networkTimeDelta / 20;
        }
    }

    void AudioLink::SendAudioOutputData() {
        GetOutputDataHelper(_audioSource, _audioFramesL, 0); // left channel

        if (_rightChannelTestCounter > 0)
        {
            if (_ignoreRightChannel)
            {
                // TODO: check if the Copy is allowed, else rewrite to memcpy
                memcpy(_audioFramesR.begin(), _audioFramesL.begin(), sizeof(float) * 4092);
                //System::Array::Copy((System::Array*)_audioFramesL.convert(), 0, (System::Array*)_audioFramesR.convert(), 0, 4092);
            }
            else {
                GetOutputDataHelper(_audioSource, _audioFramesR, 1); // right channel
            }

            _rightChannelTestCounter--;
        }
        else
        {
            _rightChannelTestCounter = ConfigProperties::RIGHT_CHANNEL_TEST_DELAY;      // reset test countdown
            _audioFramesR[0] = 0.0f;                                                    // reset tested array element to zero just in case
            GetOutputDataHelper(_audioSource, _audioFramesR, 1);                        // right channel test
            _ignoreRightChannel = _audioFramesR[0] == 0.0f;
        }

        memcpy(_samples.begin(), _audioFramesL.begin() + (1023 * 0), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples0L, _samples);
        memcpy(_samples.begin(), _audioFramesL.begin() + (1023 * 1), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples1L, _samples);
        memcpy(_samples.begin(), _audioFramesL.begin() + (1023 * 2), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples2L, _samples);
        memcpy(_samples.begin(), _audioFramesL.begin() + (1023 * 3), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples3L, _samples);

        memcpy(_samples.begin(), _audioFramesR.begin() + (1023 * 0), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples0R, _samples);
        memcpy(_samples.begin(), _audioFramesR.begin() + (1023 * 1), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples1R, _samples);
        memcpy(_samples.begin(), _audioFramesR.begin() + (1023 * 2), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples2R, _samples);
        memcpy(_samples.begin(), _audioFramesR.begin() + (1023 * 3), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples3R, _samples);
    }

    /* these methods exist to make the things they return "readonly" */
    Sombrero::FastColor AudioLink::get_customThemeColor0() { return _customThemeColor0; }
    Sombrero::FastColor AudioLink::get_customThemeColor1() { return _customThemeColor1; }
    Sombrero::FastColor AudioLink::get_customThemeColor2() { return _customThemeColor2; }
    Sombrero::FastColor AudioLink::get_customThemeColor3() { return _customThemeColor3; }
    AudioSource* AudioLink::get_audioSource() { return _audioSource; }
    Material* AudioLink::get_audioMaterial() { return _audioMaterial; }
    ArrayW<float> AudioLink::get_audioFramesL() { return _audioFramesL; }
    ArrayW<float> AudioLink::get_audioFramesR() { return _audioFramesR; }
    ArrayW<float> AudioLink::get_samples() { return _samples; }
}
