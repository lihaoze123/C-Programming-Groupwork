#ifndef ARGS_H
#define ARGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array.h>
#include <logger.h>

typedef struct {
    char* name;
    char* value;
    char* description;
    int required;
} Argument;

typedef struct {
    Array* arguments;
    Array* parsed_arguments;
} ArgumentParser;

ArgumentParser* create_argument_parser();
int add_argument(ArgumentParser* parser, const char* name, const char* description, int required);
int parse_arguments(ArgumentParser* parser, int argc, char* argv[]);
const char* get_argument_value(ArgumentParser* parser, const char* name);
int has_argument(ArgumentParser* parser, const char* name);
void free_argument_parser(ArgumentParser* parser);

#endif // ARGS_H