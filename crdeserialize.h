#ifndef _CR_DESERIALIZE_H_
#define _CR_DESERIALIZE_H_

static cr_u32 cr_read_bytes(cr_byte8* stream, cr_byte8* buffer, cr_u32 size);
static cr_u32 cr_read_bool(cr_byte8* stream, cr_byte8* value);
static cr_u32 cr_read_char(cr_byte8* stream, cr_byte8* value);
static cr_u32 cr_read_int(cr_byte8* stream, cr_byte8* value);
static cr_u32 cr_read_float(cr_byte8* stream, cr_byte8* value);

static cr_u32 cr_read_object(cr_byte8* stream, cr_byte8** object_ptr, CR_Type* type);
static cr_u32 cr_read_object_property(cr_byte8* stream, cr_byte8** object_ptr, CR_Type* type);

static cr_u32 cr_read_bytes(cr_byte8* stream, cr_byte8* buffer, cr_u32 size) {
	cr_byte8* r = stream;
	cr_byte8* w = buffer;

	while((cr_u32) (r - stream) < size) {
		*w = *r;
		r++; w++;
	}

	return (cr_u32) (r - stream);
}

static cr_u32 cr_read_bool(cr_byte8* stream, cr_byte8* value) {
	assert(stream);
	assert(value);
	
	cr_byte8* at = stream;

	cr_string tmp = {};

	tmp.value = at;
	
	while(at &&
		  *at >= 97 && *at <= 122) {
		at++;
	}

	tmp.length = (cr_u32)(at - stream);

	if(tmp.length > 3 && tmp.length < 6) {
		if(tmp.value[0] == 't' &&
		   tmp.value[1] == 'r' &&
		   tmp.value[2] == 'u' &&
		   tmp.value[3] == 'e') {
			*(cr_b8*)value = true;
		} else if(tmp.value[0] == 'f' &&
				  tmp.value[1] == 'a' &&
				  tmp.value[2] == 'l' &&
				  tmp.value[3] == 's' &&
				  tmp.value[4] == 'e') {
			*(cr_b8*) value = false;
		} else {
			printf("Error: Unexpected token while parsing bool %*s\n", tmp.length, tmp.value);
		}
	}

	// at++;
	
	return(cr_u32)(at - stream);
}

static cr_u32 cr_read_char(cr_byte8* stream, cr_byte8* value) {
	assert(stream);
	assert(value);
	
	cr_byte8* at = stream;
	*value = *at;

	at++;

	return(cr_u32)(at - stream);
}

static cr_u32 cr_read_int(cr_byte8* stream, cr_byte8* value) {
	assert(stream);
	assert(value);
	
	cr_byte8* at = stream;

	cr_string tmp = {};

	tmp.value = at;
	
	while(at && (*at == '-' || *at >= 48 && *at <= 57)) {
		at++;
	}

	tmp.length = (cr_u32)(at - stream);

	sscanf(tmp.value, "%i", (cr_s64*)value);

    // at++;
	
	return(cr_u32)(at - stream);	
}

static cr_u32 cr_read_float(cr_byte8* stream, cr_byte8* value) {
	assert(stream);
	assert(value);
	
	cr_byte8* at = stream;

	cr_string tmp = {};

	tmp.value = at;
	
	while(at && (*at == '.' || *at == '-' || (*at >= 48 && *at <= 57))) {
		at++;
	}

	tmp.length = (cr_u32)(at - stream);

	sscanf(tmp.value, "%f", (cr_r32*)value);

	// at++;
	
	return(cr_u32)(at - stream);	
}

static cr_u32 cr_read_object_property(cr_byte8* stream, cr_byte8** object_ptr, CR_Type* type) {	
	cr_byte8* at = stream;

	if(*at != '[')  {
		printf("Warning: Expecting \'%c\' at \"%*s\"\n", '[', at);
	} else {
		at++;
	}

	cr_string property_name = {};
	property_name.value = at;
	
	while(*at != '\0' &&
		  ((*at >= 97  && *at <= 122) ||
		   (*at >= 65  && *at <= 90) ||
		   (*at >= 48  && *at <= 57) ||
		   *at == '_')) {
		at++;
	}

	property_name.length = (cr_u32) (at - property_name.value);

	CR_Property* property = nullptr;

	for(cr_u32 property_index = 0; property_index < type->properties_count; ++property_index) {
		CR_Property* other = &type->properties[property_index];
		if(other->name == property_name) {
			property = other;
			break;
		}
	}

	if(!property) {
		printf("Error: Failed to find property \'%*s\' in type \'%s\'\n\n",
			   property_name.length,
			   property_name.value,
			   type->name.value);

		cr_u32 deepness = 0;

		while(at && *at) {
			if(*at == '[') {
				deepness++;
			} else if(*at == ']') {
				if(!deepness) {
					at++;
					break;
				} else {
					deepness--;
				}
			}

			at++;
		}
		
		return(cr_u32)(at - stream);
	}
	
	CR_Type* property_type = property->type;
	assert(property_type);

	if(property->deserialize_overload) {
		u32 offset = (u32) (at - stream);
		return offset + property->deserialize_overload(at, object_ptr, type, property);
	}
	
	if(*at == ']') {
		// Null or default value.

		// TODO: Zero initialization for array!
		cr_zero_mem((*object_ptr + property->offset), property_type->size);
	} else if(*at == '(') {
		at++;
		cr_u32 array_size;

		// NOTE: Handle EOF?
		
		if(*at >= 48 && *at <= 71) {
			sscanf(at, "%i", &array_size);
			while(*at != '\0' && *at >= 48 && *at <= 71) at++;
		} else {
			printf("Warning: Expecting array size at \"%s\" (backtrace: \"%s\")\n", at, stream);
			array_size = 0;
		}

		if(*at != ')') {
			printf("Warning: Expecting \'%c\' at \"%s\" (backtrace: \"%s\")\n", ')', at, stream);
		} else {
			at++;
		}
		
		if(*at != ':') {
			printf("Warning: Expecting \'%c\' at \"%s\" (backtrace: \"%s\")\n", ':', at, stream);
		} else {
			at++;
		}
		
		cr_byte8* array_ptr;
		
		if(property->is_dynamic_array) {
			cr_byte8** tmp_ptr = (cr_byte8**)(*object_ptr + property->offset);
			// *tmp_ptr = (cr_byte8*) cr_alloc(array_size * property->type->size);

			// TODO: NOTE: RP(09.03.2018):
			// In order to null terminate string we need to allocate more memory.
			// Don't have time to think on the good solution so here you go.
			
			if(property->type->name == "char" && array_size) {
				*tmp_ptr = (cr_byte8*) cr_alloc((array_size+1) * property->type->size);
				cr_zero_mem(*tmp_ptr, (array_size+1) * property->type->size);
			} else if(array_size) {
				*tmp_ptr = (cr_byte8*) cr_alloc(array_size * property->type->size);
				cr_zero_mem(*tmp_ptr, array_size * property->type->size);
			} else {
				*tmp_ptr = nullptr;
			}
			
			array_ptr = *tmp_ptr;
		} else {
			if(array_size != property->size) {
				printf("Warning: Array size (%i) \"%s.%s\" doesn't match type's prop's size (%i)! Behaving as if %i size was declared.\n",
					   array_size,
					   property_type->name.value,
					   property->name.value,
					   property->size,
					   property->size);

				array_size = property->size;
			}
			
			array_ptr = (*object_ptr + property->offset);
		}

		if(property_type->name == "bool") {
			for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
				cr_byte8* array_element = array_ptr + (property_type->size * array_index);
				at += cr_read_bool(at, array_element);
				if(array_size && array_index < array_size - 1) {
					if(*at == ',') at++;
				}
			}
		} else if(property_type->name == "int") {
			for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
				cr_byte8* array_element = array_ptr + (property_type->size * array_index);
				at += cr_read_int(at, array_element);
				if(array_size && array_index < array_size - 1) {
					if(*at == ',') at++;
				}
			}
		} else if(property_type->name == "float") {
			for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
				cr_byte8* array_element = array_ptr + (property_type->size * array_index);
				at += cr_read_float(at, array_element);
				if(array_size && array_index < array_size - 1) {
					if(*at == ',') at++;
				}
			}
		} else if(property_type->name == "char") {
			for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
				cr_byte8* array_element = array_ptr + (property_type->size * array_index);
				at += cr_read_char(at, array_element);
			}

			if(array_size) {
				*(array_ptr + (property_type->size * array_size)) = '\0';
			}
		} else {
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!	
			b8 type_is_ptr = (property->type->name.value[property->type->name.length - 1] == '*');
			
			for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
				// cr_byte8* array_element = array_ptr + (property_type->size * array_index);

				cr_byte8* array_element;
				
				if(type_is_ptr) {
					/*
					array_element = array_ptr + (property->type->size * array_index);
					cr_byte8** tmp = &array_element;
					at += cr_read_object(at, tmp, property_type);
					*((cr_byte8**)(array_ptr + (property->type->size * array_index))) = *tmp;
					*/
					cr_byte8* tmp = 0;
					at += cr_read_object(at, &tmp, property_type);
					*((cr_byte8**)(array_ptr + (property->type->size * array_index))) = tmp;
				} else {
					array_element = array_ptr + (property->type->size * array_index);
					at += cr_read_object(at, &array_element, property_type);
				}			
				
				if(array_size != 0 && array_index < array_size - 1) {
					if(*at == ',') at++;
				}
			}

			// TODO: HACK: This call eats ending character which we expect to meet later!
			if(array_size && !type_is_ptr) {
				at--;
			}
		}
	} else {
		if(property->is_dynamic_array || property->size > 1) {
			printf("Error: Expecting array at \"%s\" (backtrace: \"%s\")\n", at, stream);
			return(cr_u32) (at - stream);
		}

		if(*at != ':') {
			printf("Warning: Expecting \'%c\' at \"%s\" (backtrace: \"%s\")\n", ':', at, stream);
		} else {
			at++;
		}		
		
		cr_byte8* value_ptr = (*object_ptr + property->offset);
		
		if(property_type->name == "char") {
			at += cr_read_char(at, value_ptr);
		} else if(property_type->name == "int") {
			at += cr_read_int(at, value_ptr);
		} else if(property_type->name == "float") {
			at += cr_read_float(at, value_ptr);
		} else if(property_type->name == "bool") {
			at += cr_read_bool(at, value_ptr);
		} else {
			// at += cr_read_object(at, &value_ptr, property_type);
			// NOTE: HACK: RP(04.03.2018): Pointer work around!

			if(property_type->name.value[property_type->name.length-1] == '*') {
				cr_byte8** field_ptr = (cr_byte8**) value_ptr;
				cr_byte8*  ptr_value = *field_ptr;
				
				at += cr_read_object(at, &ptr_value, property_type);
				*field_ptr = ptr_value;
			} else {
				at += cr_read_object(at, &value_ptr, property_type);
				// TODO: HACK: This call eats ending character which we expect to meet later!
				at--;
			}
		}		
	}

	if(*at == ']') {
		at++;
	} else {
		printf("Warning: Expecting \'%c\' at \"%s\" (backtrace: \"%s\")\n", ']', at, stream);
	}
	
	return(cr_u32)(at - stream);
}


// TODO: It's possible to return actuall type. Why not?

static cr_u32 cr_read_object(cr_byte8* stream, cr_byte8** object_ptr, CR_Type* type) {
	assert(object_ptr);
	assert(type);

	if(type->deserialize_overload) {
		return type->deserialize_overload(stream, object_ptr, type);
	}
	
	cr_byte8* at = stream;

	while(at &&
		  *at != '\0' &&
		  *at != ']'  &&
		  *at != ',') {
		if(*at == '[') {
			at++;

			// Read Type's name "[typename..."
			
			cr_string type_name = {};
			
			type_name.value = at;

			while(*at != '\0' &&
				  ((*at >= 97  && *at <= 122) ||
				   (*at >= 65  && *at <= 90) ||
				   (*at >= 48  && *at <= 57) ||
				   *at == '_')) {
				at++;
			}
			
			type_name.length = (cr_u32) (at - type_name.value);

			if(type_name != type->name) {
				/*
				printf("Error: Missmatched object type \"%s\", while expecting \"%*s\"\n!",
					   type->name.value,
					   type_name.length,
					   type_name.value);
				break;
				*/

				// TODO: NOTE: Update missmatched type?
				CR_Type* new_type = cr_types_table_find_type_sized(type_name.value, type_name.length);
				if(new_type) {
					type = new_type;
				}
			}

			// ALLOCATION: Fill object ptr with new object
			
			if(!*object_ptr) {
				*object_ptr = (cr_byte8*) cr_alloc(type->size);
				cr_zero_mem(*object_ptr, type->size);
			}
			
			if(*at == '[') {
				// Reading properties
				
				while(*at == '[') {
					at += cr_read_object_property(at, object_ptr, type);
				}

			} else if(*at == ']') {
				// End of the object!
				at++;
				break;
			} else {
				printf("Unexpected token \'%c\' at \"%*s...\"\n\n", *at, 255, at);
				break;
			}
		}

		at++;
	}

	if(*at == ']') {
		at++;
	}
	
	return(cr_u32) (at - stream);
}

#endif
