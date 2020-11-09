#ifndef _CR_SERAILIZE_H_
#define _CR_SERAILIZE_H_

#define WRITE_EMPTIES 0

static cr_u32 cr_puts(cr_byte8** stream, cr_u32 at, cr_byte8* data, cr_u32 data_size);
static cr_u32 cr_write(cr_byte8** stream, cr_u32 at, cr_byte8* data, cr_u32 data_size);

static cr_u32 cr_write_bytes_t(cr_byte8** stream, cr_u32 at, cr_byte8* data, u32 count);
static cr_u32 cr_write_bytes(cr_byte8** stream, cr_u32 at, cr_byte8* data);
static cr_u32 cr_write_bool(cr_byte8** stream, cr_u32 at, cr_byte8* value);
static cr_u32 cr_write_float(cr_byte8** stream, cr_u32 at, cr_byte8* value);
static cr_u32 cr_write_int(cr_byte8** stream, cr_u32 at, cr_byte8* value);
static cr_u32 cr_write_char(cr_byte8** stream, cr_u32 at, cr_byte8* value);
static cr_u32 cr_write_property(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Property* property, CR_Type* type);
static cr_u32 cr_write_array_property(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Property* property, CR_Type* type);
static cr_u32 cr_write_object(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Type* type);

static cr_u32 cr_puts(cr_byte8** stream, cr_u32 at, cr_byte8* data, cr_u32 data_size) {
	cr_byte8* write = *(stream) + at;
	cr_byte8* read  = data;
	
	while(read - data < data_size) {
		*write = *read;
		write++;
		read++;
	}

	return (cr_u32) (write - *stream); 
}

static cr_u32 cr_write(cr_byte8** stream, cr_u32 at, cr_byte8* data, cr_u32 data_size) {
	*stream = (cr_byte8* )cr_realloc(*stream, (at + data_size));

	cr_byte8* write = *(stream) + at;
	cr_byte8* read  = data;
	
	while(read - data < data_size) {
		*write = *read;
		write++;
		read++;
	}

	return (cr_u32) (write - *stream);	
}

static cr_u32 cr_write_bytes_t(cr_byte8** stream, cr_u32 at, cr_byte8* data, u32 count) {
	*stream = (cr_byte8* )cr_realloc(*stream, sizeof(cr_byte8) * (at + count));

	cr_byte8* write = *(stream) + at;
	cr_byte8* read  = data;
	
	while(read - data < count) {
		*write = *read;
		write++;
		read++;
	}

	return (cr_u32) (write - *stream);
}
 
static cr_u32 cr_write_bytes(cr_byte8** stream, cr_u32 at, cr_byte8* data) {	
	cr_byte8* size_counter = data;
	while(*size_counter != '\0') size_counter++;

	cr_u32 count = (cr_u32) (size_counter - data);
	
	*stream = (cr_byte8*) cr_realloc(*stream, sizeof(cr_byte8) * (at + count + 1));

	cr_byte8* write = *(stream) + at;
	cr_byte8* read  = data;
	
	while(read - data < count) {
		*write = *read;
		write++;
		read++;
	}

	*write = '\0';

	return (cr_u32) (write - *stream);
}

static cr_u32 cr_write_char(cr_byte8** stream, cr_u32 at, cr_byte8* value) {
	cr_byte8 tmp_buff[2] = {
		*value, '\0'
	};
	at = cr_write_bytes(stream, at, tmp_buff);
	return(at);
}

static cr_u32 cr_write_float(cr_byte8** stream, cr_u32 at, cr_byte8* value) {
	cr_byte8 tmp_buff[128];
	sprintf(tmp_buff, "%f", *((cr_r32*)value));
	at = cr_write_bytes(stream, at, tmp_buff);
	return(at);
}

static cr_u32 cr_write_int(cr_byte8** stream, cr_u32 at, cr_byte8* value) {
	cr_byte8 tmp_buff[128];
	sprintf(tmp_buff, "%i", *((cr_s64*)value));
	at = cr_write_bytes(stream, at, tmp_buff);
	return(at);
}

static cr_u32 cr_write_bool(cr_byte8** stream, cr_u32 at, cr_byte8* value) {
	at = cr_write_bytes(stream, at, *((cr_b8*)value) ? "true" : "false");
	return(at);
}

inline bool cr_is_memory_zero(cr_byte8* bytes, u32 size) {
	cr_byte8* r = bytes;
	
	if(r - bytes < size) {
		if(*r) {
			return(false);
		}
		r++;
	}

	return(true);
}

static cr_u32 cr_write_property(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Property* property, CR_Type* type) {
	assert(object);

	cr_byte8* field_ptr = (object + property->offset);
	
	// cr_byte8* value = (object + property->offset);
	// cr_byte8* value = *((cr_byte8**) field_ptr);

	// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
	// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
	// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
	// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
	// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
	// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!	
	
	cr_byte8* value;
	if(property->type->name.value[property->type->name.length - 1] == '*') {
		value = *((cr_byte8**) field_ptr); 
	} else {
		value = (object + property->offset);
	}
	
	if(!value) return(at);

#if WRITE_EMPTIES
	if(cr_is_memory_zero(value, property->type->size)) {
		return(at);
	}
#endif
	
	at = cr_write_bytes(stream, at, "[");
	
	at = cr_write_bytes(stream, at, cr_to_c(&property->name));	
	at = cr_write_bytes(stream, at, ":");

	if(property->type == cr_char_type) {
		at = cr_write_char(stream, at, value);
	} else if(property->type == cr_int_type) {
		at = cr_write_int(stream, at, value);
	} else if(property->type == cr_float_type) {
		at = cr_write_float(stream, at, value);
	} else if(property->type == cr_bool_type) {
		at = cr_write_bool(stream, at, value);
	} else {
		at = cr_write_object(stream, at, value, property->type);
	}
	
	at = cr_write_bytes(stream, at, "]");
	
	return(at);
}

static cr_u32 cr_write_array_property(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Property* property, CR_Type* type) {	
	at = cr_write_bytes(stream, at, "[");
	
	at = cr_write_bytes(stream, at, cr_to_c(&property->name));	

	cr_byte8* array_value;
	cr_u32    array_size;
	
	if(property->is_dynamic_array) {
		array_size  = *(cr_s64*)(object + property->dynamic_array_length_offset);
		array_value = *(cr_byte8**) (object + property->offset);
	} else {
		array_size  = property->size;
		array_value = object + property->offset;
	}
	
	cr_byte8 tmp_buffer[128];
	sprintf(tmp_buffer, "(%i)", array_size);

	at = cr_write_bytes(stream, at, tmp_buffer);

	at = cr_write_bytes(stream, at, ":");
	
	if(property->type == cr_char_type) {
		for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
			cr_byte8* array_element = array_value + (array_index * property->type->size);
			at = cr_write_char(stream, at, array_element);
		}		
	} else if(property->type == cr_int_type) {
		for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
			cr_byte8* array_element = array_value + (array_index * property->type->size);
			
			at = cr_write_int(stream, at, array_element);
			
			if(array_size > 1 && array_index < array_size - 1) {
				at = cr_write_bytes(stream, at, ",");
			}
		}			
	} else if(property->type == cr_float_type) {
		for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
			cr_byte8* array_element = array_value + (array_index * property->type->size);
			
			at = cr_write_float(stream, at, array_element);
			
			if(array_size > 1 && array_index < array_size - 1) {
				at = cr_write_bytes(stream, at, ",");
			}
		}	
	} else if(property->type == cr_bool_type) {
		for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
			cr_byte8* array_element = array_value + (array_index * property->type->size);
			
			at = cr_write_bool(stream, at, array_element);
			
			if(array_size > 1 && array_index < array_size - 1) {
				at = cr_write_bytes(stream, at, ",");
			}
		}	
	} else  {
		for(cr_u32 array_index = 0; array_index < array_size; ++array_index) {
			// cr_byte8* array_element = array_value + (array_index * property->type->size);
			
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!
			// TODO: HACK: NOTE: Property must know wether it's plain data or it's a pointer to the data!	
			
			cr_byte8* array_element;
			if(property->type->name.value[property->type->name.length - 1] == '*') {
				array_element = *((cr_byte8**) (array_value + (array_index * property->type->size)));
			} else {
				array_element = array_value + (array_index * property->type->size);
			}			

			at = cr_write_object(stream, at, array_element, property->type);
			
			if(array_size > 1 && array_index < array_size - 1) {
				at = cr_write_bytes(stream, at, ",");
			}
		}	
	}

	at = cr_write_bytes(stream, at, "]");

	return(at);
}

static cr_u32 cr_write_object(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Type* type) {
	if(type->serialize_overload) {
		at = type->serialize_overload(stream, at, object, type);
		return(at);
	}

#if WRITE_EMPTIES
	if(cr_is_memory_zero(object, type->size)) {
		return(at);
	}
#endif
	
	at = cr_write_bytes(stream, at, "[");
	at = cr_write_bytes(stream, at, cr_to_c(&type->name));

	if(object) {
		for(cr_u32 property_index = 0; property_index < type->properties_count; ++property_index) {
			CR_Property* property = &type->properties[property_index];
			
			if((property->attributes & CR_ATTRIBUTE_IGNORE) == CR_ATTRIBUTE_IGNORE ||
			   (property->attributes & CR_ATTRIBUTE_DONT_SAVE) == CR_ATTRIBUTE_DONT_SAVE ||
			   (property->attributes & CR_ATTRIBUTE_DEPRECATED) == CR_ATTRIBUTE_DEPRECATED) {
				continue;
			}

			if(property->serialize_overload) {
				at = property->serialize_overload(stream, at, object, property, type);
				continue;
			}
			
			if(property->is_dynamic_array || property->size > 1) {
				at = cr_write_array_property(stream, at, object, property, type);
			} else {
				at = cr_write_property(stream, at, object, property, type);
			}
		}
	}
	
	at = cr_write_bytes(stream, at, "]");
	
	return(at);
}

#endif
