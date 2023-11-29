#pragma once

#include <yara.h>
#include "scan/skip_dirs.h"
#include "scan/config.h"
#include "compiler/compiler_attribute.h"

#define DEFAULT_SCAN_CALLBACK default_scan_callback

typedef struct SCANNER
{
    YR_RULES *yr_rules;
    YR_COMPILER *yr_compiler;
    SCANNER_CONFIG config;
} SCANNER;

typedef struct _CALLBACK_ARGS
{
    const char *file_path;
    int current_count;
    bool verbose;

} CALLBACK_ARGS;

int scan(SCANNER *scanner) check_unused_result;
int init_scanner(SCANNER **scanner, SCANNER_CONFIG config) check_unused_result;
int exit_scanner(SCANNER **scanner) check_unused_result;
int scan_file(SCANNER *scanner, YR_CALLBACK_FUNC callback) check_unused_result;
int scan_dir(SCANNER *scanner, YR_CALLBACK_FUNC callback, int32_t __current_depth) check_unused_result;
int default_scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data, void *user_data) check_unused_result;
