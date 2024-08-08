#include "_config.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"

#if __has_include("songcore/shared/SongCore.hpp")
#include "songcore/shared/SongCore.hpp"
#define SONGCORE
#else
#warning "Building without SongCore support, capability will not be registered!"
#endif

#include "config.hpp"
#include "logging.hpp"
#include "AssetBundleManager.hpp"
#include "AudioLink.hpp"
#include "Providers/GameProvider.hpp"
#include "Providers/MenuProvider.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/ColorManagerInstaller.hpp"
#include "GlobalNamespace/MissionGameplayInstaller.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerInstaller.hpp"
#include "GlobalNamespace/MainSettingsMenuViewControllersInstaller.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/PCAppInit.hpp"
#include "GlobalNamespace/QuestAppInit.hpp"
#include "GlobalNamespace/ColorManagerInstaller.hpp"
#include "GlobalNamespace/SongPreviewPlayer.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"

#include "scotland2/shared/loader.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

MAKE_HOOK_MATCH(SongPreviewPlayer_CrossFadeTo, static_cast<void (GlobalNamespace::SongPreviewPlayer::*)(::UnityEngine::AudioClip*, float, float, float, bool, ::System::Action*)>(&GlobalNamespace::SongPreviewPlayer::CrossfadeTo), void, GlobalNamespace::SongPreviewPlayer* self, ::UnityEngine::AudioClip* audioClip, float musicVolume, float startTime, float duration, bool isDefault, ::System::Action* onFadeOutCallback) {
    INFO("SongPreviewPlayer_CrossFadeTo");
    SongPreviewPlayer_CrossFadeTo(self, audioClip, musicVolume, startTime, duration, isDefault, onFadeOutCallback);
    auto menuProvider = AudioLink::MenuProvider::get_instance();
    if (menuProvider) {
        menuProvider->SongPreviewPlayerProvide(self->_activeChannel, self->_audioSourceControllers);
    } else {
        INFO("No menu provider exists!");
    }
}

MAKE_HOOK_MATCH(ColorManagerInstaller_InstallBindings, &GlobalNamespace::ColorManagerInstaller::InstallBindings, void, GlobalNamespace::ColorManagerInstaller* self) {
    INFO("ColorManagerInstaller_InstallBindings");
    ColorManagerInstaller_InstallBindings(self);
    auto menuProvider = AudioLink::MenuProvider::get_instance();
    if (menuProvider) {
        menuProvider->ColorManagerInstallerProvide(self->_menuColorScheme);
    } else {
        INFO("No menu provider exists!");
    }
}

BS_AUDIOLINK_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;
}

BS_AUDIOLINK_EXPORT_FUNC void load() {}

BS_AUDIOLINK_EXPORT_FUNC void late_load() {
    il2cpp_functions::Init();

    if (!LoadConfig()) SaveConfig();
    custom_types::Register::AutoRegister();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install(Lapiz::Zenject::Location::Player, [](::Zenject::DiContainer* container){
        container->BindInterfacesTo<AudioLink::GameProvider*>()->AsSingle()->NonLazy();
    });
    zenjector->Install(Lapiz::Zenject::Location::App, [](::Zenject::DiContainer* container){
        Lapiz::Zenject::ZenjectExtensions::FromNewComponentOnNewGameObject(container->BindInterfacesAndSelfTo<AudioLink::AssetBundleManager*>())->AsSingle();
        container->BindInterfacesAndSelfTo<AudioLink::AudioLink*>()->AsSingle();
    });
    zenjector->Install(Lapiz::Zenject::Location::Menu, [](::Zenject::DiContainer* container){
        container->Bind<AudioLink::MenuProvider*>()->AsSingle()->NonLazy();
    });

    static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID "_Install_Hooks");    \
    INSTALL_HOOK(logger, SongPreviewPlayer_CrossFadeTo);
    INSTALL_HOOK(logger, ColorManagerInstaller_InstallBindings);

#ifdef SONGCORE
    // Register installed so maps could use this as a suggestion, or sabers could check if it was installed
    SongCore::API::Capabilities::RegisterCapability("AudioLink");
#endif
}
