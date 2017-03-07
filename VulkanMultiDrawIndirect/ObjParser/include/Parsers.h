#pragma once


#include "ArfData.h"

int __declspec(dllexport) ParseObj(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers);
int __declspec(dllexport) ParseObj(const void* rawData, size_t size, ArfData::Data* data, ArfData::DataPointers* dataPointers);