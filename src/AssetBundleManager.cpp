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

        _bundle = assetBundle_LoadFromMemory(IncludedAssets::Bundle, 0);
        UnityEngine::Object::DontDestroyOnLoad(_bundle);
        _material = _bundle->LoadAsset<UnityEngine::Material*>("assets/audiolink/materials/mat_audiolink.mat");
        UnityEngine::Object::DontDestroyOnLoad(_material);
        _renderTexture = _bundle->LoadAsset<UnityEngine::RenderTexture*>("assets/audiolink/rendertextures/rt_audiolink.asset");
        UnityEngine::Object::DontDestroyOnLoad(_renderTexture);
    }

    void AssetBundleManager::RestartGame() {
        getLogger().info("AssetBundleManager RestartGame");
        if (_bundle && _bundle->m_CachedPtr.m_value) {
            _bundle->Unload(true);
        }

        _bundle = nullptr;

        if (_material && _material->m_CachedPtr.m_value) {
            UnityEngine::Object::DestroyImmediate(_material);
        }
        _material = nullptr;

        if (_renderTexture && _renderTexture->m_CachedPtr.m_value) {
            UnityEngine::Object::DestroyImmediate(_renderTexture);
        }
        _renderTexture = nullptr;
    }
}