#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/RenderTexture.hpp"

#include <functional>

DECLARE_CLASS_CODEGEN_INTERFACES(AudioLink, AssetBundleManager, Il2CppObject, classof(System::IDisposable*),
    DECLARE_PRIVATE_FIELD(UnityEngine::AssetBundle*, _bundle);
    DECLARE_PRIVATE_FIELD(UnityEngine::Material*, _material);
    DECLARE_PRIVATE_FIELD(UnityEngine::RenderTexture*, _renderTexture);

    DECLARE_INSTANCE_METHOD(UnityEngine::Material*, get_material);
    DECLARE_INSTANCE_METHOD(UnityEngine::RenderTexture*, get_renderTexture);
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&System::IDisposable::Dispose>::get());
    public:
        void Load();
        DECLARE_DEFAULT_CTOR();
    private:
)
