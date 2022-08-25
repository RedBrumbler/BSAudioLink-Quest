#include "AudioLink.hpp"
#include "AssetBundleManager.hpp"

#include "ConfigProperties.hpp"
#include "ShaderProperties.hpp"

#include "UnityEngine/Time.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Rendering/RenderTextureSubElement.hpp"
#include <chrono>

#include "System/Array.hpp"
DEFINE_TYPE(AudioLink, AudioLink);

using namespace UnityEngine;

void GetOutputDataHelper(UnityEngine::AudioSource* instance, ArrayW<float>& out, int channel) {
            // TODO: test
    using GetOutputDataHelperMethod = function_ptr_t<void, UnityEngine::AudioSource*, ArrayW<float>, int>;
    static auto getOutputDataHelper = reinterpret_cast<GetOutputDataHelperMethod>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::GetOutputDataHelper"));
    getOutputDataHelper(instance, out, channel);
}

extern Logger& getLogger();

namespace AudioLink {

    void AudioLink::ctor(Zenject::DiContainer* container) {
        getLogger().info("AudioLink ctor");
        
        auto audioTimeSyncController = container->TryResolve<GlobalNamespace::AudioTimeSyncController*>();
        auto colorScheme = container->TryResolve<GlobalNamespace::ColorScheme*>();
        getLogger().info("audioTimeSyncController: %p", audioTimeSyncController);
        getLogger().info("colorScheme: %p", colorScheme);
        
        _audioFramesL = ArrayW<float>(1023 * 4);
        _audioFramesR = ArrayW<float>(1023 * 4);
        _samples = ArrayW<float>(1023);

        _audioSource = audioTimeSyncController ? audioTimeSyncController->audioSource : nullptr;

        _customThemeColor0 = colorScheme ? colorScheme->get_environmentColor0() : Sombrero::FastColor::red();
        _customThemeColor1 = colorScheme ? colorScheme->get_environmentColor1() : Sombrero::FastColor::cyan();
        _customThemeColor2 = colorScheme ? colorScheme->get_environmentColor0Boost() : Sombrero::FastColor::pink();
        _customThemeColor3 = colorScheme ? colorScheme->get_environmentColor1Boost() : Sombrero::FastColor::lightblue();

        AssetBundleManager::get_instance()->StartLoad(std::bind(&AudioLink::Initialize, this));
    }

    void AudioLink::Initialize() {
        getLogger().info("AudioLink Initialize");
        auto manager = AssetBundleManager::get_instance();
        _audioMaterial = manager->get_material();
        auto audioRenderTexture = manager->get_renderTexture();

        UpdateSettings();
        UpdateThemeColors();


        getLogger().info("SetGlobalRenderTexture");
        Shader::SetGlobalTexture(ShaderProperties::_audioTexture, audioRenderTexture, Rendering::RenderTextureSubElement::Default);
        _initialized = true;
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
            // TODO: test
            using GetSpatialBlend = function_ptr_t<float, UnityEngine::AudioSource*>;
            static auto get_spatialBlend = reinterpret_cast<GetSpatialBlend>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::GetSpatialBlendMix"));
            _audioMaterial->SetFloat(ShaderProperties::_sourceSpatialBlend, get_spatialBlend(_audioSource));
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
        _audioMaterial->SetVector(ShaderProperties::_versionNumberAndFPSProperty, Vector4(ConfigProperties::AUDIOLINK_VERSION_NUMBER, 0, _fPSCount, 1));
        _audioMaterial->SetVector(ShaderProperties::_playerCountAndData, {0, 0, 0, 0});
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
            else
                GetOutputDataHelper(_audioSource, _audioFramesR, 1); // right channel

            _rightChannelTestCounter--;
        }
        else
        {
            _rightChannelTestCounter = ConfigProperties::RIGHT_CHANNEL_TEST_DELAY;      // reset test countdown
            _audioFramesR[0] = 0.0f;                                  // reset tested array element to zero just in case
            GetOutputDataHelper(_audioSource, _audioFramesR, 1); // right channel test
            _ignoreRightChannel = _audioFramesR[0] == 0.0f;
        }

        // TODO: check if the Copy is allowed, else rewrite to memcpy
        memcpy(_samples.begin(), _audioFramesL.begin(), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples0L, _samples);
        memcpy(_samples.begin(), _audioFramesL.begin() + 1023, sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples1L, _samples);
        memcpy(_samples.begin(), _audioFramesL.begin() + 2046, sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples2L, _samples);
        memcpy(_samples.begin(), _audioFramesL.begin() + 3069, sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples3L, _samples);

        memcpy(_samples.begin(), _audioFramesR.begin(), sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples0R, _samples);
        memcpy(_samples.begin(), _audioFramesR.begin() + 1023, sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples1R, _samples);
        memcpy(_samples.begin(), _audioFramesR.begin() + 2046, sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples2R, _samples);
        memcpy(_samples.begin(), _audioFramesR.begin() + 3069, sizeof(float) * 1023);
        _audioMaterial->SetFloatArray(ShaderProperties::_samples3R, _samples);

        /*
        System::Array::Copy((System::Array*)_audioFramesL.convert(), 0, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 4
        _audioMaterial->SetFloatArray(ShaderProperties::_samples0L, _samples);
        System::Array::Copy((System::Array*)_audioFramesL.convert(), 1023, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 3
        _audioMaterial->SetFloatArray(ShaderProperties::_samples1L, _samples);
        System::Array::Copy((System::Array*)_audioFramesL.convert(), 2046, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 2
        _audioMaterial->SetFloatArray(ShaderProperties::_samples2L, _samples);
        System::Array::Copy((System::Array*)_audioFramesL.convert(), 3069, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 1
        _audioMaterial->SetFloatArray(ShaderProperties::_samples3L, _samples);

        System::Array::Copy((System::Array*)_audioFramesR.convert(), 0, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 4
        _audioMaterial->SetFloatArray(ShaderProperties::_samples0R, _samples);
        System::Array::Copy((System::Array*)_audioFramesR.convert(), 1023, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 3
        _audioMaterial->SetFloatArray(ShaderProperties::_samples1R, _samples);
        System::Array::Copy((System::Array*)_audioFramesR.convert(), 2046, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 2
        _audioMaterial->SetFloatArray(ShaderProperties::_samples2R, _samples);
        System::Array::Copy((System::Array*)_audioFramesR.convert(), 3069, (System::Array*)_samples.convert(), 0, 1023); // 4092 - 1023 * 1
        _audioMaterial->SetFloatArray(ShaderProperties::_samples3R, _samples);
        */
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
