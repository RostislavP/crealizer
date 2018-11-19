#pragma once

#include "crcommon.h"

struct cr_string {
	cr_byte8* value;
	cr_u32    length;
};

cr_s32    cr_get_string_length(cr_byte8* string);
cr_string cr_make_string(cr_byte8* value);
cr_string cr_make_string(cr_byte8* value, cr_u32 length);
void      cr_free_string(cr_string* string);
cr_byte8* cr_to_c(cr_string* string);
cr_string cr_empty_string();
cr_string cr_copy_string(cr_string* other);

cr_b8 cr_compare_strings(cr_byte8* lhs, cr_byte8* rhs);

cr_b8 operator==(const cr_string& lhs, cr_byte8* rhs);
cr_b8 operator!=(const cr_string& lhs, cr_byte8* rhs);

cr_b8 operator==(const cr_string& lhs, const cr_string& rhs);
cr_b8 operator!=(const cr_string& lhs, const cr_string& rhs);
