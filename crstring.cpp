#include "crstring.h"
#include <stdlib.h>
#include <assert.h>

cr_s32 cr_get_string_length(cr_byte8* string) {
	if(!string) return(0);
	
	cr_s32 length = 0;
	
	while(true) {
		cr_byte8 c = string[length];
		if(c == '\0') break;
		length++;
	}

	return(length);
}

cr_string cr_make_string(cr_byte8* value) {
	cr_string string;

	if(!value) {
		string.value  = 0;
		string.length = 0;
		return string;
	} else {
		cr_s32 value_length = cr_get_string_length(value);
		string.value = (cr_byte8*)cr_alloc(sizeof(cr_byte8) * (value_length + 1));
		
		for(cr_s32 i = 0; i < value_length; ++i) {
			string.value[i] = value[i];
		}
		
		string.value[value_length] = '\0';
		string.length = value_length;	
	}
	
	return string;
}

cr_string cr_make_string(cr_byte8* value, cr_u32 length) {
	cr_string string;

	if(!value) {
		string.value  = 0;
		string.length = 0;
		return string;
	} else {
		string.value = (cr_byte8*)cr_alloc(sizeof(cr_byte8) * (length + 1));
		
		for(cr_s32 i = 0; i < length; ++i) {
			string.value[i] = value[i];
		}
		
		string.value[length] = '\0';
		string.length = length;
	}
	
	return string;
}

void cr_free_string(cr_string* string) {
	if(string->value) cr_free(string->value);
	
	string->value  = 0;
	string->length = 0;
}

cr_byte8* cr_to_c(cr_string* string) {
	return string->value;
}

cr_string cr_empty_string() {
	cr_string string;
	string.value = 0;
	string.length = 0;
	return string;
}

cr_b8 operator==(const cr_string& lhs, cr_byte8* rhs) {
	if(!lhs.value && rhs || lhs.value && !rhs)
		return(false);
	if(!lhs.value && !rhs)
		return(true);
	
	cr_s32 i = 0;
	while(rhs[i] != '\0') {
		if(i > lhs.length)
			return(false);
		if(lhs.value[i] != rhs[i])
			return(false);
		i++;
	}

	return(lhs.length == i);
}

cr_b8 operator!=(const cr_string& lhs, cr_byte8* rhs) {
	return !(lhs == rhs);
}

cr_b8 operator==(const cr_string& lhs, const cr_string& rhs) {
	cr_s32 lhs_length = lhs.length;
	cr_s32 rhs_length = rhs.length;

	if(lhs_length != rhs_length) return(false);

	for(cr_s32 i = 0; i < lhs_length; ++i) {
		if(lhs.value[i] != rhs.value[i])
			return(false);
	}

	return(true);
}

cr_b8 operator!=(const cr_string& lhs, const cr_string& rhs) {
	return !(lhs == rhs);
}

cr_b8 cr_compare_strings(cr_byte8* lhs, cr_byte8* rhs) {
	if(!lhs || !rhs) return(false);
	
	cr_s32 lhs_length = cr_get_string_length(lhs);
	cr_s32 rhs_length = cr_get_string_length(rhs);

	if(lhs_length != rhs_length)
		return(false);

	for(cr_s32 i = 0; i < lhs_length; ++i) {
		if(lhs[i] != rhs[i])
			return(false);
	}

	return(true);
}

cr_string copy_string(cr_string* other) {
	assert(other);

	if(other->length == 0) return cr_empty_string();
	
	cr_string result;
	result.value = (cr_byte8*) cr_alloc(sizeof(cr_byte8) * (other->length + 1));
	
	for(cr_s32 i = 0; i < other->length; ++i)
		result.value[i] = other->value[i];
	
	result.length = other->length;
	result.value[result.length] = '\0';

	return(result);
}
