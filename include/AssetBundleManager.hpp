#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/RenderTexture.hpp"

#include <functional>

#define DECLARE_INSTANCE_FIELD_PRIVATE(type_, name_) \
private: \
struct ___FieldRegistrator_##name_ : ::custom_types::FieldRegistrator { \
    ___FieldRegistrator_##name_() { \
        ___TargetType::___TypeRegistration::addField(this); \
    } \
    constexpr const char* name() const override { \
        return #name_; \
    } \
    const Il2CppType* type() const override { \
        ::il2cpp_functions::Init(); \
        return ::il2cpp_functions::class_get_type(::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<type_>::get()); \
    } \
    constexpr uint16_t fieldAttributes() const override { \
        return FIELD_ATTRIBUTE_PRIVATE; \
    } \
    constexpr size_t size() const override { \
        return sizeof(type_); \
    } \
    int32_t offset() const override { \
        return offsetof(___TargetType, name_); \
    } \
}; \
static inline ___FieldRegistrator_##name_ ___##name_##_FieldRegistrator; \
type_ name_

DECLARE_CLASS_CODEGEN(AudioLink, AssetBundleManager, Il2CppObject,
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AssetBundle*, _bundle);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Material*, _material);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::RenderTexture*, _renderTexture);

    DECLARE_INSTANCE_METHOD(UnityEngine::Material*, get_material);
    DECLARE_INSTANCE_METHOD(UnityEngine::RenderTexture*, get_renderTexture);
    public:
        static AssetBundleManager* get_instance();
        void StartLoad(std::function<void()> onFinished);
        void RestartGame();
        DECLARE_DEFAULT_CTOR();
    private:
        static SafePtr<AssetBundleManager> instance;
        custom_types::Helpers::Coroutine LoadAsync(std::function<void()> onFinished);
)

#undef DECLARE_INSTANCE_FIELD_PRIVATE