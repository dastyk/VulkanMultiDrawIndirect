#pragma once


#include "ArfData.h"

#define PARSER_FILE_NOT_FOUND -1
#define PARSER_SUCCESS 0
#define INTERLEAVE 1

int ParseObj(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);
int ParseObj(const void* rawData, size_t size, ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);

int ParseArf(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);

int Interleave(ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);