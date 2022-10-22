#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/RenderTexture.hpp"

#include <functional>

DECLARE_CLASS_CODEGEN(AudioLink, AssetBundleManager, Il2CppObject,
    DECLARE_PRIVATE_FIELD(UnityEngine::AssetBundle*, _bundle);
    DECLARE_PRIVATE_FIELD(UnityEngine::Material*, _material);
    DECLARE_PRIVATE_FIELD(UnityEngine::RenderTexture*, _renderTexture);

    DECLARE_INSTANCE_METHOD(UnityEngine::Material*, get_material);
    DECLARE_INSTANCE_METHOD(UnityEngine::RenderTexture*, get_renderTexture);
    public:
        static AssetBundleManager* get_instance();
        void Load();
        void RestartGame();
        DECLARE_DEFAULT_CTOR();
    private:
        static SafePtr<AssetBundleManager> instance;
)