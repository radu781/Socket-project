#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"

void *allocatePtr(size_t typeSize, size_t amount)
{
    void *obj = malloc(amount * typeSize);
    memset(obj, '\0', amount * typeSize);
    logDebug("Allocated %d bytes at %p", amount * typeSize, obj);
    return obj;
}

void **allocatePtrPtr(size_t typeSize, size_t ptrSize, size_t cols, size_t rows)
{
    void **obj = (void **)malloc(rows * ptrSize);
    logDebug("Allocated %d bytes at %p", rows * typeSize, obj);
    for (int i = 0; i < rows; i++)
    {
        obj[i] = malloc(cols * typeSize);
        memset(obj[i], '\0', cols * typeSize);
        logDebug("Allocated %d bytes at %p", cols * typeSize, obj[i], i);
    }
    return obj;
}

void deallocatePtr(void *obj)
{
    free(obj);
    logDebug("Deallocated at %p", obj);
    obj = NULL;
}

void deallocatePtrPtr(void **obj, size_t rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(obj[i]);
        logDebug("Deallocated at %p", obj[i]);
        obj[i] = NULL;
    }
    free(obj);
    logDebug("Deallocated at %p", obj);
    obj = NULL;
}
