#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include "AssetBundleManager.hpp"
#include "AudioLink.hpp"

#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/MissionGameplayInstaller.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerInstaller.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}


MAKE_HOOK_MATCH(StandardGameplayInstaller_InstallBindings, &GlobalNamespace::StandardGameplayInstaller::InstallBindings, void, GlobalNamespace::StandardGameplayInstaller* self) {
    StandardGameplayInstaller_InstallBindings(self);
}

MAKE_HOOK_MATCH(MissionGameplayInstaller_InstallBindings, &GlobalNamespace::MissionGameplayInstaller::InstallBindings, void, GlobalNamespace::MissionGameplayInstaller* self) {
    MissionGameplayInstaller_InstallBindings(self);
}

MAKE_HOOK_MATCH(MultiplayerLocalActivePlayerInstaller_InstallBindings, &GlobalNamespace::MultiplayerLocalActivePlayerInstaller::InstallBindings, void, GlobalNamespace::MultiplayerLocalActivePlayerInstaller* self) {
    MultiplayerLocalActivePlayerInstaller_InstallBindings(self);
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
    
    PinkCore::RequirementAPI::RegisterInstalled("AudioLink");
}