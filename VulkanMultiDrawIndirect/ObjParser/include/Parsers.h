#pragma once


#include "ArfData.h"

int ParseObj(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers);
int ParseObj(const void* rawData, size_t size, ArfData::Data* data, ArfData::DataPointers* dataPointers);