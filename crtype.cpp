#include "crcommon.h"
#include "crtype.h"

inline cr_u32 cr_make_hash_from_string(cr_byte8* in_string) {
   assert(in_string);
   
   cr_u32 hash_sum = 24863511448;
   cr_u32     hash = hash_sum;
   
   cr_byte8* in_r = in_string;
   
   while(*in_r) {	   
	   hash ^= (cr_u32) *in_r;
	   
	   hash += (hash << 10);
	   hash -= (hash << 6);
	   
	   in_r++;
   }

   hash += (hash << 3);
   hash ^= (hash << 11);
   hash += (hash << 15);
   
   return(hash);
}

void cr_type_add_property(CR_Type* type, cr_byte8* name, cr_u16 offset, CR_Type* value_type, u64 attributes) {
	type->properties_count++;
	type->properties = (CR_Property*) cr_realloc(type->properties, sizeof(CR_Property) * type->properties_count);
	
	CR_Property* new_property = &type->properties[type->properties_count-1];
	*new_property = {};

	new_property->name   = cr_make_string(name);
	new_property->offset = offset;
	new_property->size   = 1;
	new_property->type   = value_type;
	
	new_property->attributes = attributes;
}

void cr_type_add_dynamic_array_property(CR_Type* type, cr_byte8* name, cr_u16 offset, cr_u16 length_offset, CR_Type* value_type, u64 attributes) {
	type->properties_count++;
	type->properties = (CR_Property*) cr_realloc(type->properties, sizeof(CR_Property) * type->properties_count);
	
	CR_Property* new_property = &type->properties[type->properties_count-1];
	*new_property = {};

	new_property->name   = cr_make_string(name);
	new_property->offset = offset;
	new_property->size   = 0;
	new_property->type   = value_type;
	
	new_property->attributes = attributes;
	
	new_property->is_dynamic_array = true;
	new_property->dynamic_array_length_offset = length_offset;
}

void cr_type_add_static_array_property(CR_Type* type, cr_byte8* name, cr_u16 offset, cr_u32 size, CR_Type* value_type, u64 attributes) {
	type->properties_count++;
	type->properties = (CR_Property*) cr_realloc(type->properties, sizeof(CR_Property) * type->properties_count);
	
	CR_Property* new_property = &type->properties[type->properties_count-1];
	*new_property = {};
	
	new_property->name   = cr_make_string(name);
	new_property->offset = offset;
	new_property->size   = size;
	new_property->type   = value_type;
	
	new_property->attributes = attributes;
}

cr_s32 cr_type_has_property(CR_Type* type, cr_byte8* name) {
	cr_s32 result = -1;
	
	for(u32 i = 0; i < type->properties_count; ++i) {
		if(type->properties[i].name == name) {
			result = i;
			break;
		}
	}

	return(result);
}

CR_Property* cr_find_property(CR_Type* type, cr_byte8* name) {
	for(u32 i = 0; i < type->properties_count; ++i) {
		if(type->properties[i].name == name) {
			return(&type->properties[i]);
		}
	}

	return(0);	
}

void cr_default_types_init() {
	cr_char_type = {};
	cr_char_type.name      = cr_make_string("char");
	cr_char_type.name_hash = cr_make_hash_from_string("char");
	cr_char_type.size      = sizeof(cr_byte8);

	cr_float_type = {};
	cr_float_type.name      = cr_make_string("float");
	cr_float_type.name_hash = cr_make_hash_from_string("float");
	cr_float_type.size      = sizeof(cr_r32);
	
	cr_int_type = {};
	cr_int_type.name      = cr_make_string("int");
	cr_int_type.name_hash = cr_make_hash_from_string("int");	
	cr_int_type.size      = sizeof(cr_s32);

	cr_bool_type = {};
	cr_bool_type.name      = cr_make_string("bool");
	cr_bool_type.name_hash = cr_make_hash_from_string("bool");
	cr_bool_type.size      = sizeof(cr_b8);
}

void cr_types_table_init() {
	cr_types_table.count = 4;
	cr_types_table.types = (CR_Type**) cr_realloc(cr_types_table.types, sizeof(CR_Type*) * 4);

	cr_types_table.types[0] = &cr_char_type;
	cr_types_table.types[1] = &cr_int_type;
	cr_types_table.types[2] = &cr_float_type;
	cr_types_table.types[3] = &cr_bool_type;
}

CR_Type* cr_types_table_new_type(cr_byte8* name, cr_u32 size) {
	cr_types_table.count++;
	cr_types_table.types = (CR_Type**) cr_realloc(cr_types_table.types, sizeof(CR_Type*) * cr_types_table.count);

	CR_Type* new_type = (CR_Type*) cr_alloc(sizeof(CR_Type));
	*new_type = {};
	
	new_type->name      = cr_make_string(name);
	new_type->name_hash = cr_make_hash_from_string(name);
	new_type->size      = size;

	cr_types_table.types[cr_types_table.count-1] = new_type;

	return new_type;
}

CR_Type* cr_types_table_find_type(cr_byte8* name) {
	/*
	for(cr_u32 i = 0; i < cr_types_table.count; ++i) {
		CR_Type* type = cr_types_table.types[i];
		if(type->name == name) {
			return type;
		}
	}

	return(nullptr);
	*/

	cr_u32 name_hash = cr_make_hash_from_string(name);

	for(cr_u32 i = 0; i < cr_types_table.count; ++i) {
		CR_Type* type = cr_types_table.types[i];
		if(type->name_hash == name_hash) {
			// ASSERT(type->name == name, "Hash error!\n");
			return type;
		}
	}

	return(nullptr);	
}

CR_Type* cr_types_table_find_type_sized(cr_byte8* name, cr_u32 name_size) {
	cr_string str;
	str.value  = name;
	str.length = name_size;

	for(cr_u32 i = 0; i < cr_types_table.count; ++i) {
		CR_Type* type = cr_types_table.types[i];
		if(type->name == str) {
			return type;
		}
	}

	return(nullptr);
}
