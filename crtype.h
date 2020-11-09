#pragma once

#include "crcommon.h"

typedef struct CR_Type CR_Type;
typedef struct CR_Property CR_Property;

typedef cr_u32 (CR_PROPERTY_SERIALIZE_OVERLOAD_FNC)(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Property* property, CR_Type* type);
typedef cr_u32 (CR_PROPERTY_DESERIALIZE_OVERLOAD_FNC)(cr_byte8* stream, cr_byte8** object_ptr, CR_Type* type, CR_Property* property);

typedef cr_u32 (CR_TYPE_SERIALIZE_OVERLOAD_FNC)(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Type* type);
typedef cr_u32 (CR_TYPE_DESERIALIZE_OVERLOAD_FNC)(cr_byte8* stream, cr_byte8** object, CR_Type* type);

typedef void (CR_TYPE_COPY_OVERLOAD_FNC)(cr_u8** object_ptr, cr_u8* copy, cr_u32 properties_offset, CR_Type* type);

// Attributes

#define CR_ATTRIBUTE_NONE       0x0000
#define CR_ATTRIBUTE_HIDE       0x0001
#define CR_ATTRIBUTE_IGNORE     0x0002
#define CR_ATTRIBUTE_COPY_NULL  0x0004
#define CR_ATTRIBUTE_DEPRECATED 0x0008
#define CR_ATTRIBUTE_DONT_SAVE  0x0010
#define CR_CUSTOM_ATTRIBUTE_0   0x0020
#define CR_CUSTOM_ATTRIBUTE_1   0x0040
#define CR_CUSTOM_ATTRIBUTE_2   0x0080
#define CR_CUSTOM_ATTRIBUTE_3   0x0100
#define CR_CUSTOM_ATTRIBUTE_4   0x0200
#define CR_CUSTOM_ATTRIBUTE_5   0x0400
#define CR_CUSTOM_ATTRIBUTE_6   0x0800
#define CR_CUSTOM_ATTRIBUTE_7   0x1000
#define CR_CUSTOM_ATTRIBUTE_8   0x2000
#define CR_CUSTOM_ATTRIBUTE_9   0x4000
#define CR_CUSTOM_ATTRIBUTE_10  0x8000

struct CR_Property {
	cr_string name;
	cr_u16    offset;
	cr_u32    size;
	CR_Type*  type;
	
	cr_b8     is_dynamic_array;
	cr_u16    dynamic_array_length_offset;

	cr_u64 attributes;

	CR_PROPERTY_SERIALIZE_OVERLOAD_FNC*     serialize_overload;
	CR_PROPERTY_DESERIALIZE_OVERLOAD_FNC* deserialize_overload;
};

struct CR_Type {
	cr_string    name;
	cr_u32       name_hash;
	
	cr_u32       size;
	CR_Property* properties;
	cr_u32       properties_count;

	CR_TYPE_SERIALIZE_OVERLOAD_FNC*   serialize_overload;
	CR_TYPE_DESERIALIZE_OVERLOAD_FNC* deserialize_overload;
	CR_TYPE_COPY_OVERLOAD_FNC*        copy_overload;

	void* user_data0;
};

struct {
	CR_Type** types;
	cr_u32    count;
} static cr_types_table = {};

static CR_Type* cr_int_type;
static CR_Type* cr_float_type;
static CR_Type* cr_char_type;
static CR_Type* cr_bool_type;

void cr_default_types_init();

void   cr_type_add_property(CR_Type* type, cr_byte8* name, cr_u16 offset, CR_Type* value_type, cr_u64 attributes);
void   cr_type_add_dynamic_array_property(CR_Type* type, cr_byte8* name, cr_u16 offset, cr_u16 length_offset, CR_Type* value_type, cr_u64 attributes);
void   cr_type_add_static_array_property(CR_Type* type, cr_byte8* name, cr_u16 offset, cr_u32 size, CR_Type* value_type, cr_u64 attributes);
cr_s32 cr_type_has_property(CR_Type* type, cr_byte8* name);
CR_Property* cr_find_property(CR_Type* type, cr_byte8* name);
													
void     cr_types_table_init();
CR_Type* cr_types_table_new_type(cr_byte8* name, cr_u32 size);
CR_Type* cr_types_table_find_type(cr_byte8* name);
CR_Type* cr_types_table_find_type_sized(cr_byte8* name, cr_u32 name_size);
