#include "config.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

config_t config;

extern Logger& getLogger();
Configuration& get_config() {
    static Configuration config({MOD_ID, VERSION});
    config.Load();
    return config;
}

#define Save(identifier) doc.AddMember(#identifier, config.identifier, allocator)

void SaveConfig() {
    getLogger().info("Saving Configuration...");
    rapidjson::Document& doc = get_config().config;

    doc.RemoveAllMembers();
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    Save(showTestPlane);
    get_config().Write();
    getLogger().info("Saved Configuration!");
}

#define GET_BOOL(identifier)                                            \
    auto identifier##_itr = doc.FindMember(#identifier);                \
    if (identifier##_itr != doc.MemberEnd())                            \
        config.identifier = identifier##_itr->value.GetBool();          \
    else                                                                \
        foundEverything = false;

bool LoadConfig() {
    getLogger().info("Loading Configuration...");
    bool foundEverything = true;
    rapidjson::Document& doc = get_config().config;

    GET_BOOL(showTestPlane);

    if (foundEverything)
        getLogger().info("Loaded Configuration!");
    return foundEverything;
}