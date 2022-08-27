#include "AssetBundleManager.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"

#include "assets.hpp"

DEFINE_TYPE(AudioLink, AssetBundleManager);

extern Logger& getLogger();

namespace AudioLink {
    SafePtr<AssetBundleManager> AssetBundleManager::instance;
    
    AssetBundleManager* AssetBundleManager::get_instance() {
        if (!instance) {
            instance = AssetBundleManager::New_ctor();
        }
        return instance.ptr();
    }

    UnityEngine::Material* AssetBundleManager::get_material() {
        return _material;
    }

    UnityEngine::RenderTexture* AssetBundleManager::get_renderTexture() {
        return _renderTexture;
    }

    void AssetBundleManager::Load() {
        if (_bundle && _bundle->m_CachedPtr.m_value) return;
        using AssetBundle_LoadFromMemory = function_ptr_t<UnityEngine::AssetBundle*, ArrayW<uint8_t>, int>;
        static auto assetBundle_LoadFromMemory = reinterpret_cast<AssetBundle_LoadFromMemory>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemory_Internal"));
    
        static auto _bundle_info = il2cpp_functions::class_get_field_from_name(klass, "_bundle");
        static auto _material_info = il2cpp_functions::class_get_field_from_name(klass, "_material");
        static auto _renderTexture_info = il2cpp_functions::class_get_field_from_name(klass, "_renderTexture");

        il2cpp_functions::field_set_value_object(this, _bundle_info, assetBundle_LoadFromMemory(IncludedAssets::Bundle, 0));
        il2cpp_functions::field_set_value_object(this, _material_info, _bundle->LoadAsset<UnityEngine::Material*>("assets/audiolink/materials/mat_audiolink.mat"));

        auto rt = _bundle->LoadAsset("assets/audiolink/rendertextures/rt_audiolink.asset");
        il2cpp_functions::field_set_value_object(this, _renderTexture_info, rt);
    }

    void AssetBundleManager::RestartGame() {
        getLogger().info("AssetBundleManager RestartGame");
        static auto _bundle_info = il2cpp_functions::class_get_field_from_name(klass, "_bundle");
        static auto _material_info = il2cpp_functions::class_get_field_from_name(klass, "_material");
        static auto _renderTexture_info = il2cpp_functions::class_get_field_from_name(klass, "_renderTexture");

        if (_bundle && _bundle->m_CachedPtr.m_value) {
            _bundle->Unload(true);
        }

        il2cpp_functions::field_set_value_object(this, _bundle_info, nullptr);

        if (_material && _material->m_CachedPtr.m_value) {
            UnityEngine::Object::DestroyImmediate(_material);
        }
        il2cpp_functions::field_set_value_object(this, _material_info, nullptr);

        if (_renderTexture && _renderTexture->m_CachedPtr.m_value) {
            UnityEngine::Object::DestroyImmediate(_renderTexture);
        }
        il2cpp_functions::field_set_value_object(this, _renderTexture_info, nullptr);
    }
}