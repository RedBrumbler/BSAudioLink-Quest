#pragma once

#include "UnityEngine/Shader.hpp"


extern Logger& getLogger();

namespace ShaderProperties {
    struct ShaderPropID {
        template<size_t sz>
        constexpr ShaderPropID(const char (&str)[sz]) : str(str), sz(sz) {}

        operator int () const {
            if (id == 0) {
                id = UnityEngine::Shader::PropertyToID(std::string_view(str, sz)); 
                getLogger().info("Shader Prop '%s': %d", str, id);
            }
            return id;
        }

        private:
            mutable int id = 0;
            const size_t sz;
            const char* str;
    };

    // properties based on https://github.com/Aeroluna/BSAudioLink/blob/master/AudioLink/Scripts/AudioLink.cs
    static const ShaderPropID _audioTexture("_AudioTexture");
    static const ShaderPropID _versionNumberAndFPSProperty("_VersionNumberAndFPSProperty");
    static const ShaderPropID _playerCountAndData("_PlayerCountAndData");
    static const ShaderPropID _advancedTimeProps("_AdvancedTimeProps");
    static const ShaderPropID _advancedTimeProps2("_AdvancedTimeProps2");
    static const ShaderPropID _sourceVolume("_SourceVolume");
    static const ShaderPropID _sourceSpatialBlend("_SourceSpatialBlend");
    static const ShaderPropID _x0("_X0");
    static const ShaderPropID _x1("_X1");
    static const ShaderPropID _x2("_X2");
    static const ShaderPropID _x3("_X3");
    static const ShaderPropID _threshold0("_Threshold0");
    static const ShaderPropID _threshold1("_Threshold1");
    static const ShaderPropID _threshold2("_Threshold2");
    static const ShaderPropID _threshold3("_Threshold3");
    static const ShaderPropID _gain("_Gain");
    static const ShaderPropID _fadeLength("_FadeLength");
    static const ShaderPropID _fadeExpFalloff("_FadeExpFalloff");
    static const ShaderPropID _bass("_Bass");
    static const ShaderPropID _treble("_Treble");
    static const ShaderPropID _themeColorMode("_ThemeColorMode");
    static const ShaderPropID _customThemeColor0ID("_CustomThemeColor0");
    static const ShaderPropID _customThemeColor1ID("_CustomThemeColor1");
    static const ShaderPropID _customThemeColor2ID("_CustomThemeColor2");
    static const ShaderPropID _customThemeColor3ID("_CustomThemeColor3");
    static const ShaderPropID _samples0L("_Samples0L");
    static const ShaderPropID _samples1L("_Samples1L");
    static const ShaderPropID _samples2L("_Samples2L");
    static const ShaderPropID _samples3L("_Samples3L");
    static const ShaderPropID _samples0R("_Samples0R");
    static const ShaderPropID _samples1R("_Samples1R");
    static const ShaderPropID _samples2R("_Samples2R");
    static const ShaderPropID _samples3R("_Samples3R");
}