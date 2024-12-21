#include "config.h"

static ConfigItem* create_config_item(const char* section, const char* key, const char* value) {
    ConfigItem* item = (ConfigItem*)malloc(sizeof(ConfigItem));
    if (!item) {
        log_message(LOG_ERROR, "申请内存失败");
        return NULL;
    }

    item->section = strdup(section);
    item->key = strdup(key);
    item->value = strdup(value);

    if (!item->section || !item->key || !item->value) {
        log_message(LOG_ERROR, "申请内存失败");
        free(item->section);
        free(item->key);
        free(item->value);
        free(item);
        return NULL;
    }

    return item;
}

static void free_config_item(ConfigItem* item) {
    if (item) {
        free(item->section);
        free(item->key);
        free(item->value);
        free(item);
    }
}

Config* create_config(const char* filename) {
    Config* config = (Config*)malloc(sizeof(Config));
    if (!config) {
        log_message(LOG_ERROR, "申请内存失败");
        return NULL;
    }

    config->items = create_array(0, sizeof(ConfigItem*));
    if (!config->items) {
        log_message(LOG_ERROR, "创建数组失败");
        free(config);
        return NULL;
    }

    config->filename = strdup(filename);
    if (!config->filename) {
        log_message(LOG_ERROR, "申请内存失败");
        free_array(config->items);
        free(config);
        return NULL;
    }

    return config;
}

int load_config(Config* config) {
    FILE* fp = fopen(config->filename, "r");
    if (fp == NULL) {
        log_message(LOG_ERROR, "打开配置文件失败: %s", config->filename);
        return -1;
    }

    char line[256];
    char current_section[64] = "";
    
    while (fgets(line, sizeof(line), fp)) {
        char* newline = strchr(line, '\n');
        if (newline) *newline = '\0';

        if (line[0] == '\0' || line[0] == ';' || line[0] == '#')
            continue;

        if (line[0] == '[') {
            char* end = strchr(line, ']');
            if (end) {
                *end = '\0';
                strncpy(current_section, line + 1, sizeof(current_section) - 1);
                current_section[sizeof(current_section) - 1] = '\0';
            }
            continue;
        }

        char* eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char* key = line;
            char* value = eq + 1;

            while (*key == ' ') key++;
            while (*value == ' ') value++;
            char* end = key + strlen(key) - 1;
            while (end > key && *end == ' ') *end-- = '\0';
            end = value + strlen(value) - 1;
            while (end > value && *end == ' ') *end-- = '\0';

            ConfigItem* item = create_config_item(current_section, key, value);
            if (item) {
                add_element(config->items, &item);
            }
        }
    }

    fclose(fp);
    return 0;
}

int save_config(Config* config) {
    FILE* fp = fopen(config->filename, "w");
    if (fp == NULL) {
        log_message(LOG_ERROR, "打开配置文件失败: %s", config->filename);
        return -1;
    }

    char current_section[64] = "";

    for (size_t i = 0; i < config->items->size; ++i) {
        ConfigItem* item = *(ConfigItem**)at(config->items, i);
        
        if (strcmp(current_section, item->section) != 0) {
            fprintf(fp, "[%s]\n", item->section);
            strncpy(current_section, item->section, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
        }

        fprintf(fp, "%s = %s\n", item->key, item->value);
    }

    fclose(fp);
    return 0;
}

const char* get_config_value(Config* config, const char* section, const char* key) {
    for (size_t i = 0; i < config->items->size; ++ i) {
        ConfigItem* item = *(ConfigItem**)at(config->items, i);
        if (strcmp(item->section, section) == 0 && strcmp(item->key, key) == 0) {
            return item->value;
        }
    }
    return NULL;
}

int set_config_value(Config* config, const char* section, const char* key, const char* value) {
    for (size_t i = 0; i < config->items->size; ++ i) {
        ConfigItem* item = *(ConfigItem**)at(config->items, i);
        if (strcmp(item->section, section) == 0 && strcmp(item->key, key) == 0) {
            char* new_value = strdup(value);
            if (!new_value) {
                log_message(LOG_ERROR, "申请内存失败");
                return -1;
            }
            free(item->value);
            item->value = new_value;
            return 0;
        }
    }

    ConfigItem* new_item = create_config_item(section, key, value);
    if (!new_item) {
        return -1;
    }

    return add_element(config->items, &new_item);
}

void free_config(Config* config) {
    if (config) {
        for (size_t i = 0; i < config->items->size; ++ i) {
            ConfigItem* item = *(ConfigItem**)at(config->items, i);
            free_config_item(item);
        }
        
        free_array(config->items);
        free(config->filename);
        free(config);
    }
} 