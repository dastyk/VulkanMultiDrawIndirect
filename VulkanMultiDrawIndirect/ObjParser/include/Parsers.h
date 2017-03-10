#pragma once


#include "ArfData.h"

#define OBJ_PARSER_FILE_NOT_FOUND -1

int ParseObj(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers);
int ParseObj(const void* rawData, size_t size, ArfData::Data* data, ArfData::DataPointers* dataPointers);