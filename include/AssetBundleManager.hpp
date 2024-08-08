#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "System/IDisposable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(AudioLink, AssetBundleManager, System::Object, classof(System::IDisposable*),
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AssetBundle*, _bundle);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Material*, _material);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::RenderTexture*, _renderTexture);

    DECLARE_INSTANCE_METHOD(UnityEngine::Material*, get_material);
    DECLARE_INSTANCE_METHOD(UnityEngine::RenderTexture*, get_renderTexture);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);
    public:
        void Load();
        DECLARE_DEFAULT_CTOR();
    private:
)
