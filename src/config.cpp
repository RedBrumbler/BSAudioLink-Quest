#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

config_t config;

Configuration& get_config() {
    static Configuration config({MOD_ID, VERSION, 0});
    config.Load();
    return config;
}

#define Save(identifier) doc.AddMember(#identifier, config.identifier, allocator)

void SaveConfig() {
    INFO("Saving Configuration...");
    rapidjson::Document& doc = get_config().config;

    doc.RemoveAllMembers();
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    Save(showTestPlane);
    get_config().Write();
    INFO("Saved Configuration!");
}

#define GET_BOOL(identifier)                                            \
    auto identifier##_itr = doc.FindMember(#identifier);                \
    if (identifier##_itr != doc.MemberEnd())                            \
        config.identifier = identifier##_itr->value.GetBool();          \
    else                                                                \
        foundEverything = false;

bool LoadConfig() {
    INFO("Loading Configuration...");
    bool foundEverything = true;
    rapidjson::Document& doc = get_config().config;

    GET_BOOL(showTestPlane);

    if (foundEverything)
        INFO("Loaded Configuration!");
    return foundEverything;
}
