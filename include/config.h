#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array.h>
#include <logger.h>

typedef struct ConfigItem {
    char* section;
    char* key;
    char* value;
} ConfigItem;

typedef struct Config {
    Array* items;
    char* filename;
} Config;

Config* create_config(const char* filename);
int load_config(Config* config);
int save_config(Config* config);
const char* get_config_value(Config* config, const char* section, const char* key);
int set_config_value(Config* config, const char* section, const char* key, const char* value);
void free_config(Config* config);

#endif // CONFIG_H