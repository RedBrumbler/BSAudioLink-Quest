#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include "AssetBundleManager.hpp"
#include "AudioLink.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/MissionGameplayInstaller.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerInstaller.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

void Install(Zenject::MonoInstaller* self) {
    auto container = self->get_Container();
    container->BindInterfacesTo<AudioLink::AudioLink*>()->AsSingle()->NonLazy();
}


MAKE_HOOK_MATCH(StandardGameplayInstaller_InstallBindings, &GlobalNamespace::StandardGameplayInstaller::InstallBindings, void, GlobalNamespace::StandardGameplayInstaller* self) {
    StandardGameplayInstaller_InstallBindings(self);
    Install(self);
}

MAKE_HOOK_MATCH(MissionGameplayInstaller_InstallBindings, &GlobalNamespace::MissionGameplayInstaller::InstallBindings, void, GlobalNamespace::MissionGameplayInstaller* self) {
    MissionGameplayInstaller_InstallBindings(self);
    Install(self);
}

MAKE_HOOK_MATCH(MultiplayerLocalActivePlayerInstaller_InstallBindings, &GlobalNamespace::MultiplayerLocalActivePlayerInstaller::InstallBindings, void, GlobalNamespace::MultiplayerLocalActivePlayerInstaller* self) {
    MultiplayerLocalActivePlayerInstaller_InstallBindings(self);
    Install(self);
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &GlobalNamespace::MenuTransitionsHelper::RestartGame, void, GlobalNamespace::MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback) {
    AudioLink::AssetBundleManager::get_instance()->RestartGame();
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    custom_types::Register::AutoRegister();

    auto& logger = getLogger();
    INSTALL_HOOK(logger, StandardGameplayInstaller_InstallBindings);
    INSTALL_HOOK(logger, MissionGameplayInstaller_InstallBindings);
    INSTALL_HOOK(logger, MultiplayerLocalActivePlayerInstaller_InstallBindings);
    INSTALL_HOOK(logger, MenuTransitionsHelper_RestartGame);
    
    PinkCore::RequirementAPI::RegisterInstalled("AudioLink");
}