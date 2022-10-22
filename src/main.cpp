#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"

#if __has_include("pinkcore/shared/RequirementAPI.hpp")
#include "pinkcore/shared/RequirementAPI.hpp"
#define PINKCORE
#endif

#include "config.hpp"
#include "AssetBundleManager.hpp"
#include "AudioLink.hpp"
#include "Providers/GameProvider.hpp"
#include "Providers/MenuProvider.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/MissionGameplayInstaller.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerInstaller.hpp"
#include "GlobalNamespace/MainSettingsMenuViewControllersInstaller.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/PCAppInit.hpp"
#include "GlobalNamespace/QuestAppInit.hpp"
#include "GlobalNamespace/ColorManagerInstaller.hpp"
#include "GlobalNamespace/SongPreviewPlayer.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &GlobalNamespace::MenuTransitionsHelper::RestartGame, void, GlobalNamespace::MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback) {
    getLogger().info("MenuTransitionsHelper_RestartGame");
    AudioLink::AssetBundleManager::get_instance()->RestartGame();
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

MAKE_HOOK_MATCH(SongPreviewPlayer_CrossFadeTo, static_cast<void (GlobalNamespace::SongPreviewPlayer::*)(::UnityEngine::AudioClip*, float, float, float, bool, ::System::Action*)>(&GlobalNamespace::SongPreviewPlayer::CrossfadeTo), void, GlobalNamespace::SongPreviewPlayer* self, ::UnityEngine::AudioClip* audioClip, float musicVolume, float startTime, float duration, bool isDefault, ::System::Action* onFadeOutCallback) {
    getLogger().info("SongPreviewPlayer_CrossFadeTo");
    SongPreviewPlayer_CrossFadeTo(self, audioClip, musicVolume, startTime, duration, isDefault, onFadeOutCallback);
    auto menuProvider = AudioLink::MenuProvider::get_instance();
    if (menuProvider) {
        menuProvider->SongPreviewPlayerProvide(self->activeChannel, self->audioSourceControllers);
    } else {
        getLogger().info("No menu provider exists!");
    }
}

MAKE_HOOK_MATCH(ColorManagerInstaller_InstallBindings, &GlobalNamespace::ColorManagerInstaller::InstallBindings, void, GlobalNamespace::ColorManagerInstaller* self) {
    getLogger().info("ColorManagerInstaller_InstallBindings");
    ColorManagerInstaller_InstallBindings(self);
    auto menuProvider = AudioLink::MenuProvider::get_instance();
    if (menuProvider) {
        menuProvider->ColorManagerInstallerProvide(self->menuColorScheme);
    } else {
        getLogger().info("No menu provider exists!");
    }
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    il2cpp_functions::Init();

    if (!LoadConfig()) SaveConfig();
    custom_types::Register::AutoRegister();

    auto& logger = getLogger();
    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install(Lapiz::Zenject::Location::Player, [](::Zenject::DiContainer* container){
        container->BindInterfacesTo<AudioLink::GameProvider*>()->AsSingle()->NonLazy();
    }); 
    zenjector->Install(Lapiz::Zenject::Location::App, [](::Zenject::DiContainer* container){
        container->BindInterfacesAndSelfTo<AudioLink::AudioLink*>()->AsSingle();
    });
    zenjector->Install(Lapiz::Zenject::Location::Menu, [](::Zenject::DiContainer* container){
        container->Bind<AudioLink::MenuProvider*>()->AsSingle()->NonLazy();
    });

    INSTALL_HOOK(logger, MenuTransitionsHelper_RestartGame);
    INSTALL_HOOK(logger, SongPreviewPlayer_CrossFadeTo);
    INSTALL_HOOK(logger, ColorManagerInstaller_InstallBindings);

#ifdef PINKCORE
    // Register installed so maps could use this as a suggestion, or sabers could check if it was installed
    PinkCore::RequirementAPI::RegisterInstalled("AudioLink");
#endif
}