#pragma once

#include "crcommon.h"
#include "crstring.h"
#include "crtype.h"
#include "crdeepcopy.h"

/* Serialzier by ROSTISLAV POGOSIAN: 17.01.2018 */

/* ------- TODO ------- 
- Perfomrance optimization
- Dumb char type comparassion
- Binary serialization
- Fast type and properties look ups.
- Import meta-builder (from the engine) for automatic type generation

TEXT serialization format: 

Example: 

-------------

struct vec4f {
   float x = 1.0f;
   float y = 2.0f;
   float z = 3.0f;
   float w = 4.0f;
};

[vec4f[x:1.0][y:2.0][z:3.0][w:4.0]]

-------------

struct Guy {
   char short_name[2] = "ME";
   int  health = 100;
};

[Guy[short_name(2):ME][health:100]]


------------

struct Snake_Segment {
    float x;
	float y;
};

struct Snake {
    float tail_length = 20;
	float head_size = 3.14;

	int            segments_count = 2;
	Snake_Segment* segments = malloc(sizeof(Snake_Segment) * segments_count);
};

[Snake[tail_length:20][head_size:3.14][segments_count:2][segments(2):[Snake_Segment[x:0][y:0]][Snake_Segment[x:20][y:0]]]]


-----------

Real life example

[Tag_Entity[entity:[Entity[id:[Entity_ID[scene_id:782690648][local_id:0][asset_id:[String[length:0][value(0):]]]]][type_info:[Tag_Entity]][name:[String[length:9][value(9):box_spawn]]][active:true][layer:1][team_id:0][internal:[Entity_Internal[local_position:[Vec3[x:-32.953503][y:77.412766][z:0.000000]]][local_rotation:[Quat[w:1.000000][x:0.000000][y:0.000000][z:0.000000]]][local_scale:[Vec3[x:1.000000][y:1.000000][z:1.000000]]][world_position:[Vec3[x:-32.953503][y:77.412766][z:0.000000]]][world_rotation:[Quat[w:1.000000][x:0.000000][y:0.000000][z:0.000000]]][world_scale:[Vec3[x:1.000000][y:1.000000][z:1.000000]]][children:[Entity_Array[count:0][values(0):]]]]]]]]

*/

cr_u32 cr_serialize_object(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Type* type);
cr_u32 cr_deserialize_object(cr_byte8* stream, cr_byte8** object, CR_Type* type);

/* ........................ Macro hell ........................ */

#define CR_TYPE(TYPE) cr_types_table_new_type(#TYPE, sizeof(TYPE))

#define CR_FIELD_ATTR(STRUCT, NAME, TYPE, ATTRIBUTES) cr_type_add_property(cr_types_table_find_type(#STRUCT), #NAME, offsetof(STRUCT, NAME), cr_types_table_find_type(#TYPE), ATTRIBUTES);
#define CR_FIELD(STRUCT, NAME, TYPE) CR_FIELD_ATTR(STRUCT, NAME, TYPE, 0)

#define CR_DARRAY_FIELD_ATTR(STRUCT, NAME, LENGTH_NAME, TYPE, ATTRIBUTES) cr_type_add_dynamic_array_property(cr_types_table_find_type(#STRUCT), #NAME, offsetof(STRUCT, NAME), offsetof(STRUCT, LENGTH_NAME), cr_types_table_find_type(#TYPE), ATTRIBUTES);
#define CR_DARRAY_FIELD(STRUCT, NAME, LENGTH_NAME, TYPE) CR_DARRAY_FIELD_ATTR(STRUCT, NAME, LENGTH_NAME, TYPE, 0)

#define CR_SARRAY_FIELD_ATTR(STRUCT, NAME, SIZE, TYPE, ATTRIBUTES) cr_type_add_static_array_property(cr_types_table_find_type(#STRUCT), #NAME, offsetof(STRUCT, NAME), SIZE, cr_types_table_find_type(#TYPE), ATTRIBUTES);
#define CR_SARRAY_FIELD(STRUCT, NAME, SIZE, TYPE) CR_SARRAY_FIELD_ATTR(STRUCT, NAME, SIZE, TYPE, 0)

#define CR_TYPEOF(NAME) cr_types_table_find_type(#NAME)

#define CR_TYPE_USER_DATA(TYPE, DATA) cr_types_table_find_type(#TYPE)->user_data0 = DATA

#define CR_FIELD_OF(TYPE, FIELD) cr_find_property(CR_TYPEOF(TYPE), FIELD)


// Runtime meta building macros

#define _CR_TYPE
#define _CR_FIELD(_VA_ARGS_)
#define _CR_DARRAY(ARRAY_SIZE, _VA_ARGS_)
#define _CR_ALIAS(ALIAS, TYPE)
#define _CR_IGNORE_START
#define _CR_IGNORE_END

inline void* cr_new(CR_Type* type) {
	void* data = cr_alloc(type->size);
	cr_zero_mem(data, type->size);
	return(data);
}

inline void* cr_newc(CR_Type* type, void* allocator) {
	void* data = ((CR_ALLOC*)allocator)(type->size);
	cr_zero_mem(data, type->size);
	return(data);
}

#define CR_NEW(TYPE) (cr_new(CR_TYPEOF(TYPE)))
