#pragma once

#include "nyangine/base/string.h"

extern bool nya_file_read(const char* path, NYA_String* out_content) __attr_overloaded;
extern bool nya_file_read(const NYA_String* path, NYA_String* out_content) __attr_overloaded;

extern bool nya_file_write(const char* path, const NYA_String* content) __attr_overloaded;
extern bool nya_file_write(const NYA_String* path, const NYA_String* content) __attr_overloaded;

extern bool nya_file_append(const char* path, const NYA_String* content) __attr_overloaded;
extern bool nya_file_append(const NYA_String* path, const NYA_String* content) __attr_overloaded;
