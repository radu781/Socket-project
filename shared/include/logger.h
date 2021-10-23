/**
 * Header of functions that log messages based on a predefined value given as
 * an argument with the '-D' gcc flag, where:
 * LOGGER_LEVEL == 2: all debug and communication info is logged
 * LOGGER_LEVEL == 1: all Client/Server communication info is logged
 * LOGGER_LEVEL == 0: nothing is logged
 */

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef LOGGER_LEVEL
#define LOG_ALL 2
#define LOG_SOME 1
#define LOG_NONE 0
#endif

FILE *_logger;
bool _fileExists = false;

void logSet(const char *filename)
{
    if (!_fileExists)
    {
        _logger = fopen(filename, "w");
        _fileExists = true;
    }
}
void logDebug(const char *format, ...)
{
#if (!defined(LOGGER_LEVEL) || LOGGER_LEVEL < LOG_ALL)
    return;
#elif LOGGER_LEVEL == LOG_ALL
    va_list args;
    va_start(args, format);
    va_end(args);

    fprintf(_logger, "[DEBUG] ");
    vfprintf(_logger, format, args);
    fprintf(_logger, "\n");
    fflush(_logger);
#endif
}

void logComm(FILE *where, const char *format, ...)
{
#if (!defined(LOGGER_LEVEL) || LOGGER_LEVEL == LOG_NONE)
    return;
#elif LOGGER_LEVEL >= LOG_SOME
    va_list args;
    va_start(args, format);
    va_end(args);

    fprintf(_logger, "[COMM] ");
    vfprintf(_logger, format, args);
    fprintf(_logger, "\n");
    fflush(_logger);
    
    va_start(args, format);
    va_end(args);
    
    fprintf(where, "[COMM] ");
    vfprintf(where, format, args);
    fprintf(where, "\n");
    fflush(where);
#endif
}

void logCleanUp()
{
    if (_fileExists)
    {
        fclose(_logger);
        _fileExists = false;
    }
}
