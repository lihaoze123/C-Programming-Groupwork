#include <args.h>

ArgumentParser* create_argument_parser() {
    ArgumentParser* parser = (ArgumentParser*)malloc(sizeof(ArgumentParser));
    if (!parser) {
        log_message(LOG_ERROR, "申请内存失败");
        return NULL;
    }

    parser->arguments = create_array(0, sizeof(Argument));
    if (!parser->arguments) {
        log_message(LOG_ERROR, "创建参数数组失败");
        free(parser);
        return NULL;
    }

    parser->parsed_arguments = create_array(0, sizeof(Argument));
    if (!parser->parsed_arguments) {
        log_message(LOG_ERROR, "创建解析后参数数组失败");
        free_array(parser->arguments);
        free(parser);
        return NULL;
    }

    log_message(LOG_INFO, "创建参数解析器成功");
    return parser;
}

int add_argument(ArgumentParser* parser, const char* name, const char* description, int required) {
    if (!parser || !name || !description) {
        log_message(LOG_ERROR, "添加参数失败：无效的参数");
        return 0;
    }

    for (size_t i = 0; i < parser->arguments->size; ++ i) {
        Argument* arg = (Argument*)at(parser->arguments, i);
        if (strcmp(arg->name, name) == 0) {
            log_message(LOG_WARNING, "添加参数失败：参数已存在 [参数名:%s]", name);
            return 0;
        }
    }

    Argument arg;
    arg.name = strdup(name);
    arg.description = strdup(description);
    arg.required = required;
    arg.value = NULL;

    if (!add_element(parser->arguments, &arg)) {
        log_message(LOG_ERROR, "添加参数失败：内存分配失败 [参数名:%s]", name);
        free(arg.name);
        free(arg.description);
        return 0;
    }

    log_message(LOG_INFO, "成功添加参数 [参数名:%s, 描述:%s, 必需:%d]", name, description, required);
    return 1;
}

int parse_arguments(ArgumentParser* parser, int argc, char* argv[]) {
    if (!parser || argc < 1 || !argv) {
        log_message(LOG_ERROR, "解析参数失败：无效的参数");
        return 0;
    }

    log_message(LOG_INFO, "开始解析命令行参数 [参数数量:%d]", argc - 1);

    while (parser->parsed_arguments->size > 0) {
        remove_element(parser->parsed_arguments, parser->parsed_arguments->size - 1);
    }

    for (int i = 1; i < argc; ++ i) {
        if (argv[i][0] != '-') {
            continue;
        }

        const char* name = argv[i] + 1;
        const char* value = NULL;

        if (i + 1 < argc && argv[i + 1][0] != '-') {
            value = argv[i + 1];
            i++;
        }

        int found = 0;
        for (size_t j = 0; j < parser->arguments->size; ++ j) {
            Argument* def = (Argument*)at(parser->arguments, j);
            if (strcmp(def->name, name) == 0) {
                Argument parsed_arg;
                parsed_arg.name = strdup(name);
                parsed_arg.value = value ? strdup(value) : NULL;
                parsed_arg.description = strdup(def->description);
                parsed_arg.required = def->required;

                if (!add_element(parser->parsed_arguments, &parsed_arg)) {
                    log_message(LOG_ERROR, "解析参数失败：添加解析后参数失败 [参数名:%s]", name);
                    return 0;
                }
                log_message(LOG_DEBUG, "成功解析参数 [参数名:%s, 值:%s]", name, value ? value : "NULL");
                found = 1;
                break;
            }
        }

        if (!found) {
            log_message(LOG_WARNING, "解析参数：未知参数 [参数名:%s]", name);
        }
    }

    for (size_t i = 0; i < parser->arguments->size; ++ i) {
        Argument* def = (Argument*)at(parser->arguments, i);
        if (def->required) {
            int found = 0;
            for (size_t j = 0; j < parser->parsed_arguments->size; ++ j) {
                Argument* parsed = (Argument*)at(parser->parsed_arguments, j);
                if (strcmp(def->name, parsed->name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                log_message(LOG_ERROR, "解析参数失败：缺少必需参数 [参数名:%s]", def->name);
                return 0;
            }
        }
    }

    log_message(LOG_INFO, "命令行参数解析完成 [成功解析:%zu个]", parser->parsed_arguments->size);
    return 1;
}

const char* get_argument_value(ArgumentParser* parser, const char* name) {
    if (!parser || !name) {
        log_message(LOG_ERROR, "获取参数值失败：无效的参数");
        return NULL;
    }

    for (size_t i = 0; i < parser->parsed_arguments->size; ++ i) {
        Argument* arg = (Argument*)at(parser->parsed_arguments, i);
        if (strcmp(arg->name, name) == 0) {
            log_message(LOG_DEBUG, "获取参数值 [参数名:%s, 值:%s]", name, arg->value ? arg->value : "NULL");
            return arg->value;
        }
    }

    log_message(LOG_DEBUG, "获取参数值：参数未找到 [参数名:%s]", name);
    return NULL;
}

int has_argument(ArgumentParser* parser, const char* name) {
    if (!parser || !name) {
        log_message(LOG_ERROR, "参数错误");
        return 0;
    }

    for (size_t i = 0; i < parser->parsed_arguments->size; ++ i) {
        Argument* arg = (Argument*)at(parser->parsed_arguments, i);
        if (strcmp(arg->name, name) == 0) {
            return 1;
        }
    }

    return 0;
}

void free_argument_parser(ArgumentParser* parser) {
    if (!parser) {
        return;
    }

    if (parser->arguments) {
        for (size_t i = 0; i < parser->arguments->size; ++ i) {
            Argument* arg = (Argument*)at(parser->arguments, i);
            free(arg->name);
            free(arg->description);
            if (arg->value) {
                free(arg->value);
            }
        }
        free_array(parser->arguments);
    }

    if (parser->parsed_arguments) {
        for (size_t i = 0; i < parser->parsed_arguments->size; ++ i) {
            Argument* arg = (Argument*)at(parser->parsed_arguments, i);
            free(arg->name);
            free(arg->description);
            if (arg->value) {
                free(arg->value);
            }
        }
        free_array(parser->parsed_arguments);
    }

    free(parser);
    log_message(LOG_INFO, "释放参数解析器成功");
} 