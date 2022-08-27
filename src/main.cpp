#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"

#if __has_include("pinkcore/shared/RequirementAPI.hpp")
#include "pinkcore/shared/RequirementAPI.hpp"
#define PINKCORE
#endif

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

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

bool provideMenu = true;

void InstallApp(Zenject::MonoInstaller* self) {
    getLogger().info("Install App");
    auto container = self->get_Container();
    container->BindInterfacesAndSelfTo<AudioLink::AudioLink*>()->AsSingle();
}

void InstallPlayer(Zenject::MonoInstaller* self) {
    getLogger().info("Install Player");
    provideMenu = false;
    auto container = self->get_Container();
    container->BindInterfacesTo<AudioLink::GameProvider*>()->AsSingle()->NonLazy();
}

void InstallMenu(Zenject::MonoInstaller* self) {
    getLogger().info("Install Menu");
    provideMenu = true;
    auto container = self->get_Container();
    container->Bind<AudioLink::MenuProvider*>()->AsSingle()->NonLazy();
}

MAKE_HOOK_MATCH(PCAppInit_InstallBindings, &GlobalNamespace::PCAppInit::InstallBindings, void, GlobalNamespace::PCAppInit* self) {
    getLogger().info("PCAppInit_InstallBindings");
    PCAppInit_InstallBindings(self);
    InstallApp(self);
}

MAKE_HOOK_MATCH(QuestAppInit_InstallBindings, &GlobalNamespace::QuestAppInit::InstallBindings, void, GlobalNamespace::QuestAppInit* self) {
    getLogger().info("QuestAppInit_InstallBindings");
    QuestAppInit_InstallBindings(self);
    InstallApp(self);
}


MAKE_HOOK_MATCH(StandardGameplayInstaller_InstallBindings, &GlobalNamespace::StandardGameplayInstaller::InstallBindings, void, GlobalNamespace::StandardGameplayInstaller* self) {
    getLogger().info("StandardGameplayInstaller_InstallBindings");
    StandardGameplayInstaller_InstallBindings(self);
    InstallPlayer(self);
}

MAKE_HOOK_MATCH(MissionGameplayInstaller_InstallBindings, &GlobalNamespace::MissionGameplayInstaller::InstallBindings, void, GlobalNamespace::MissionGameplayInstaller* self) {
    getLogger().info("MissionGameplayInstaller_InstallBindings");
    MissionGameplayInstaller_InstallBindings(self);
    InstallPlayer(self);
}

MAKE_HOOK_MATCH(MultiplayerLocalActivePlayerInstaller_InstallBindings, &GlobalNamespace::MultiplayerLocalActivePlayerInstaller::InstallBindings, void, GlobalNamespace::MultiplayerLocalActivePlayerInstaller* self) {
    getLogger().info("MultiplayerLocalActivePlayerInstaller_InstallBindings");
    MultiplayerLocalActivePlayerInstaller_InstallBindings(self);
    InstallPlayer(self);
}

MAKE_HOOK_MATCH(MainSettingsMenuViewControllersInstaller_InstallBindings, &GlobalNamespace::MainSettingsMenuViewControllersInstaller::InstallBindings, void, GlobalNamespace::MainSettingsMenuViewControllersInstaller* self) {
    getLogger().info("MainSettingsMenuViewControllersInstaller_InstallBindings");
    MainSettingsMenuViewControllersInstaller_InstallBindings(self);
    InstallMenu(self);
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &GlobalNamespace::MenuTransitionsHelper::RestartGame, void, GlobalNamespace::MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback) {
    getLogger().info("MenuTransitionsHelper_RestartGame");
    AudioLink::AssetBundleManager::get_instance()->RestartGame();
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

MAKE_HOOK_MATCH(SongPreviewPlayer_CrossFadeTo, static_cast<void (GlobalNamespace::SongPreviewPlayer::*)(::UnityEngine::AudioClip*, float, float, float, bool, ::System::Action*)>(&GlobalNamespace::SongPreviewPlayer::CrossfadeTo), void, GlobalNamespace::SongPreviewPlayer* self, ::UnityEngine::AudioClip* audioClip, float musicVolume, float startTime, float duration, bool isDefault, ::System::Action* onFadeOutCallback) {
    getLogger().info("SongPreviewPlayer_CrossFadeTo");
    SongPreviewPlayer_CrossFadeTo(self, audioClip, musicVolume, startTime, duration, isDefault, onFadeOutCallback);
    if (!provideMenu) {
        getLogger().info("Provide menu blocked!");
        return;
    }
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
    if (!provideMenu) {
        getLogger().info("Provide menu blocked!");
        return;
    }
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

    custom_types::Register::AutoRegister();

    auto& logger = getLogger();
    INSTALL_HOOK(logger, PCAppInit_InstallBindings);
    INSTALL_HOOK(logger, QuestAppInit_InstallBindings);
    INSTALL_HOOK(logger, StandardGameplayInstaller_InstallBindings);
    INSTALL_HOOK(logger, MissionGameplayInstaller_InstallBindings);
    INSTALL_HOOK(logger, MultiplayerLocalActivePlayerInstaller_InstallBindings);
    INSTALL_HOOK(logger, MainSettingsMenuViewControllersInstaller_InstallBindings);

    INSTALL_HOOK(logger, MenuTransitionsHelper_RestartGame);
    INSTALL_HOOK(logger, SongPreviewPlayer_CrossFadeTo);
    INSTALL_HOOK(logger, ColorManagerInstaller_InstallBindings);

#ifdef PINKCORE
    // Register installed so maps could use this as a suggestion, or sabers could check if it was installed
    PinkCore::RequirementAPI::RegisterInstalled("AudioLink");
#endif
}