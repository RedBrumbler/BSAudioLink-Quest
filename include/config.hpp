#pragma once

struct config_t {
    bool showTestPlane = false;
};

void SaveConfig();
bool LoadConfig();

extern config_t config;