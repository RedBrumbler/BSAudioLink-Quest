#pragma once

struct config_t {
    bool makeTestPlane = false;
};

void SaveConfig();
bool LoadConfig();

extern config_t config;