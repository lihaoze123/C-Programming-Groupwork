#include "config.h"

static ConfigItem* create_config_item(const char* section, const char* key, const char* value) {
    ConfigItem* item = (ConfigItem*)malloc(sizeof(ConfigItem));
    if (!item) {
        log_message(LOG_ERROR, "创建配置项失败：内存分配失败 [section:%s, key:%s]", section, key);
        return NULL;
    }

    item->section = strdup(section);
    item->key = strdup(key);
    item->value = strdup(value);

    if (!item->section || !item->key || !item->value) {
        log_message(LOG_ERROR, "创建配置项失败：字符串复制失败 [section:%s, key:%s]", section, key);
        free(item->section);
        free(item->key);
        free(item->value);
        free(item);
        return NULL;
    }

    log_message(LOG_DEBUG, "成功创建配置项 [section:%s, key:%s, value:%s]", section, key, value);
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
        log_message(LOG_ERROR, "创建配置管理器失败：内存分配失败");
        return NULL;
    }

    config->items = create_array(0, sizeof(ConfigItem*));
    if (!config->items) {
        log_message(LOG_ERROR, "创建配置管理器失败：创建数组失败");
        free(config);
        return NULL;
    }

    config->filename = strdup(filename);
    if (!config->filename) {
        log_message(LOG_ERROR, "创建配置管理器失败：复制文件名失败 [文件名:%s]", filename);
        free_array(config->items);
        free(config);
        return NULL;
    }

    log_message(LOG_INFO, "成功创建配置管理器 [文件名:%s]", filename);
    return config;
}

int load_config(Config* config) {
    FILE* fp = fopen(config->filename, "r");
    if (fp == NULL) {
        log_message(LOG_ERROR, "加载配置失败：无法打开文件 [文件名:%s]", config->filename);
        return -1;
    }

    log_message(LOG_INFO, "开始加载配置文件 [文件名:%s]", config->filename);
    char line[256];
    char current_section[64] = "";
    size_t line_number = 0;
    size_t items_loaded = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_number++;
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
                log_message(LOG_DEBUG, "解析配置节 [节名:%s, 行号:%zu]", current_section, line_number);
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
                if (add_element(config->items, &item)) {
                    items_loaded++;
                    log_message(LOG_DEBUG, "加载配置项 [section:%s, key:%s, value:%s, 行号:%zu]", 
                              current_section, key, value, line_number);
                } else {
                    log_message(LOG_ERROR, "加载配置项失败：添加元素失败 [section:%s, key:%s, 行号:%zu]", 
                              current_section, key, line_number);
                }
            }
        }
    }

    fclose(fp);
    log_message(LOG_INFO, "配置文件加载完成 [文件名:%s, 总行数:%zu, 加载项数:%zu]", 
                config->filename, line_number, items_loaded);
    return 0;
}

int save_config(Config* config) {
    FILE* fp = fopen(config->filename, "w");
    if (fp == NULL) {
        log_message(LOG_ERROR, "保存配置失败：无法创建文件 [文件名:%s]", config->filename);
        return -1;
    }

    log_message(LOG_INFO, "开始保存配置文件 [文件名:%s, 配置项数:%zu]", config->filename, config->items->size);
    char current_section[64] = "";
    size_t items_saved = 0;

    for (size_t i = 0; i < config->items->size; ++i) {
        ConfigItem* item = *(ConfigItem**)at(config->items, i);
        
        if (strcmp(current_section, item->section) != 0) {
            fprintf(fp, "[%s]\n", item->section);
            strncpy(current_section, item->section, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
            log_message(LOG_DEBUG, "写入配置节 [节名:%s]", item->section);
        }

        fprintf(fp, "%s = %s\n", item->key, item->value);
        items_saved++;
        log_message(LOG_DEBUG, "保存配置项 [section:%s, key:%s, value:%s]", 
                   item->section, item->key, item->value);
    }

    fclose(fp);
    log_message(LOG_INFO, "配置文件保存完成 [文件名:%s, 保存项数:%zu]", config->filename, items_saved);
    return 0;
}

const char* get_config_value(Config* config, const char* section, const char* key) {
    if (!config || !section || !key) {
        log_message(LOG_ERROR, "获取配置值失败：无效的参数");
        return NULL;
    }

    for (size_t i = 0; i < config->items->size; ++i) {
        ConfigItem* item = *(ConfigItem**)at(config->items, i);
        if (strcmp(item->section, section) == 0 && strcmp(item->key, key) == 0) {
            log_message(LOG_DEBUG, "获取配置值 [section:%s, key:%s, value:%s]", 
                       section, key, item->value);
            return item->value;
        }
    }

    log_message(LOG_DEBUG, "获取配置值：未找到配置项 [section:%s, key:%s]", section, key);
    return NULL;
}

int set_config_value(Config* config, const char* section, const char* key, const char* value) {
    if (!config || !section || !key || !value) {
        log_message(LOG_ERROR, "设置配置值失败：无效的参数");
        return -1;
    }

    for (size_t i = 0; i < config->items->size; ++i) {
        ConfigItem* item = *(ConfigItem**)at(config->items, i);
        if (strcmp(item->section, section) == 0 && strcmp(item->key, key) == 0) {
            char* new_value = strdup(value);
            if (!new_value) {
                log_message(LOG_ERROR, "设置配置值失败：内存分配失败 [section:%s, key:%s]", section, key);
                return -1;
            }
            free(item->value);
            item->value = new_value;
            log_message(LOG_INFO, "更新配置值 [section:%s, key:%s, 旧值:%s, 新值:%s]", 
                       section, key, item->value, value);
            return 0;
        }
    }

    ConfigItem* new_item = create_config_item(section, key, value);
    if (!new_item) {
        return -1;
    }

    if (!add_element(config->items, &new_item)) {
        log_message(LOG_ERROR, "设置配置值失败：添加新配置项失败 [section:%s, key:%s]", section, key);
        free_config_item(new_item);
        return -1;
    }

    log_message(LOG_INFO, "添加新配置项 [section:%s, key:%s, value:%s]", section, key, value);
    return 0;
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