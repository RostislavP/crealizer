#ifndef _CR_DEEP_COPY_H_
#define _CR_DEEP_COPY_H_

#include "crcommon.h"

static void cr_deep_copy(cr_u8** object_ptr, cr_u8* copy, cr_u32 properties_offset, CR_Type* type);

static void cr_deep_copy(cr_u8** object_ptr, cr_u8* copy, cr_u32 properties_offset, CR_Type* type) {
	assert(object_ptr);
	assert(type);

	if(type->copy_overload) {
		type->copy_overload(object_ptr, copy, properties_offset, type);
		return;
	}
	
	if(!copy) return;

	if(!*object_ptr) {
		*object_ptr = (cr_u8*) cr_alloc(type->size);
		cr_zero_mem(*object_ptr, type->size);
	}

	cr_u8* object = *object_ptr;

	if(type->name == "int") {
		*((cr_s32*) object) = *((cr_s32*) copy);
	} else if(type->name == "float") {
		*((cr_r32*) object) = *((cr_r32*) copy);		
	} else if(type->name == "char") {
		*((cr_byte8*) object) = *((cr_byte8*) copy);
	} else if(type->name == "bool") {
		*((cr_b8*) object) = *((cr_b8*) copy);
	} else {
		for(cr_s32 i = properties_offset; i < type->properties_count; ++i) {		
			CR_Property* property = &type->properties[i];

			// TODO: What about attributes order is copy null more important than ignore?
			// TODO: Handle other cases for ignore (pointers, arrays, etc).
			
			if((property->attributes & CR_ATTRIBUTE_COPY_NULL) == CR_ATTRIBUTE_COPY_NULL) {
				cr_u8* a_ptr = (cr_u8*)(object + property->offset);
				cr_zero_mem(a_ptr, property->type->size);
				continue;
			}
			
			if((property->attributes & CR_ATTRIBUTE_IGNORE) == CR_ATTRIBUTE_IGNORE) {
				continue;
			}
			
			if(property->is_dynamic_array || property->size > 1) {
				if(property->type->name.length > 0 && property->type->name.value[property->type->name.length-1] == '*') {
					cr_u8** a_field_ptr = (cr_u8**)(object + property->offset);
					cr_u8** b_field_ptr = (cr_u8**)(copy + property->offset);

					cr_u32 array_size;
				
					if(property->is_dynamic_array) {
						cr_u32*  a_size_ptr = (cr_u32*)(object + property->dynamic_array_length_offset);
						cr_u32*  b_size_ptr = (cr_u32*)(copy + property->dynamic_array_length_offset);
					
						*a_size_ptr = *b_size_ptr;
						array_size = *a_size_ptr;

						if(property->type->name == "char" && array_size) {
							// HACK: We want null terminated strings...
							*a_field_ptr = (cr_u8*) cr_realloc(*a_field_ptr, property->type->size * (array_size+1));
							cr_zero_mem(*a_field_ptr, property->type->size * (array_size+1));
						} else if(array_size) {
							*a_field_ptr = (cr_u8*) cr_realloc(*a_field_ptr, property->type->size * array_size);
							cr_zero_mem(*a_field_ptr, property->type->size * (array_size));
						} else {
							*a_field_ptr = 0;
						}
					} else {
						array_size = property->size;
					}

					cr_u8* ws = *a_field_ptr;
					cr_u8* we = (*a_field_ptr) + (array_size * property->type->size);
					cr_u8* w  = ws;
					cr_u8* r = *b_field_ptr;

					while(w != we) {
						*w = *r;
						w++; r++;
					}
				} else {
					cr_u8** a_field_ptr;
					cr_u8** b_field_ptr;

					cr_u32 array_size;
					if(property->is_dynamic_array) {
						a_field_ptr = (u8**) (object + property->offset);
						b_field_ptr = (u8**) (copy   + property->offset);
					
						cr_u32*  a_size_ptr = (cr_u32*)(object + property->dynamic_array_length_offset);
						cr_u32*  b_size_ptr = (cr_u32*)(copy + property->dynamic_array_length_offset);
					
						*a_size_ptr = *b_size_ptr;
						array_size = *a_size_ptr;

						if(array_size) {
							// TODO: NOTE: RP(10.03.2018): String hack fro null termination!
							if(property->type->name == "char") {
								*a_field_ptr = (cr_u8*) cr_realloc(*a_field_ptr, property->type->size * (array_size+1));
								cr_zero_mem(*a_field_ptr, property->type->size * (array_size+1));
							} else {
								*a_field_ptr = (cr_u8*) cr_realloc(*a_field_ptr, property->type->size * array_size);	
								cr_zero_mem(*a_field_ptr, property->type->size * array_size);
							}
						} else {
							*a_field_ptr = 0;
						}
					} else {
						a_field_ptr = (cr_u8**)(object + property->offset);
						b_field_ptr = (cr_u8**)(copy   + property->offset);
					
						array_size = property->size;
					}

					if(property->type->name == "float") {
						for(cr_u32 j = 0; j < array_size; ++j) {
							cr_r32* vp_a = (cr_r32*) ((*a_field_ptr) + (j * property->type->size));
							cr_r32* vp_b = (cr_r32*) ((*b_field_ptr) + (j * property->type->size));
							*vp_a = *vp_b;
						}
					} else if(property->type->name == "int") {
						for(cr_u32 j = 0; j < array_size; ++j) {
							cr_s32* vp_a = (cr_s32*) ((*a_field_ptr) + (j * property->type->size));
							cr_s32* vp_b = (cr_s32*) ((*b_field_ptr) + (j * property->type->size));
							*vp_a = *vp_b;
						}
					} else if(property->type->name == "char") {
						for(cr_u32 j = 0; j < array_size; ++j) {
							cr_u8* vp_a = (cr_u8*) ((*a_field_ptr) + (j * property->type->size));
							cr_u8* vp_b = (cr_u8*) ((*b_field_ptr) + (j * property->type->size));
							*vp_a = *vp_b;
						}

						// NOTE: Null terminate string array
					
						if(array_size) {
							*(((*a_field_ptr) + (array_size * property->type->size))) = '\0';
						}
					} else if(property->type->name == "bool") {
						for(cr_u32 j = 0; j < array_size; ++j) {
							cr_b8* vp_a = (cr_b8*) ((*a_field_ptr) + (j * property->type->size));
							cr_b8* vp_b = (cr_b8*) ((*b_field_ptr) + (j * property->type->size));
							*vp_a = *vp_b;
						}
					} else {
						if(property->is_dynamic_array) {
							for(cr_u32 j = 0; j < array_size; ++j) {
								cr_u8* e_a = ((*a_field_ptr) + (j * property->type->size));
								cr_u8* e_b = ((*b_field_ptr) + (j * property->type->size));
							
								cr_deep_copy(&e_a, e_b, 0, property->type);
							}
						} else {
							for(cr_u32 j = 0; j < array_size; ++j) {
								cr_u8* e_a = (((cr_u8*) a_field_ptr) + (j * property->type->size));
								cr_u8* e_b = (((cr_u8*) b_field_ptr) + (j * property->type->size));
							
								cr_deep_copy(&e_a, e_b, 0, property->type);
							}
						}
					}
				}
			} else {
				if(property->type->name == "float") {
					cr_r32* vp_a = (cr_r32*) (object + property->offset);
					cr_r32* vp_b = (cr_r32*) (copy   + property->offset);
					*vp_a = *vp_b;
				} else if(property->type->name == "int") {
					cr_s32* vp_a = (cr_s32*) (object + property->offset);
					cr_s32* vp_b = (cr_s32*) (copy   + property->offset);
					*vp_a = *vp_b;
				} else if(property->type->name == "bool") {
					cr_b8* vp_a = (cr_b8*) (object + property->offset);
					cr_b8* vp_b = (cr_b8*) (copy   + property->offset);
					*vp_a = *vp_b;
				} else if(property->type->name == "char") {
					cr_u8* vp_a = (cr_u8*) (object + property->offset);
					cr_u8* vp_b = (cr_u8*) (copy   + property->offset);
					*vp_a = *vp_b;
				} else {
					if(property->type->name.value[property->type->name.length-1] == '*') {
						cr_u8* ws = object + property->offset;
						cr_u8* w = ws;
						cr_u8* r = copy + property->offset;

						while(w - ws < property->type->size) {
							*w = *r; w++; r++;
						}
					} else {
						cr_u8* field_a = (object + property->offset);
						cr_u8* field_b = (copy   + property->offset);
					
						cr_u8** field_a_ptr = &field_a;
						cr_u8** field_b_ptr = &field_b;
					
						cr_deep_copy(field_a_ptr, *field_b_ptr, 0, property->type);
					}
				}
			}
		}
	}
}

#endif


// TODO: NOTE: cr_deep_free ??
