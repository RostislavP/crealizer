#pragma once

#include "crcommon.h"
#include "crstring.h"
#include "crtype.h"
#include "crdeepcopy.h"

/* Serialzier by ROSTISLAV POGOSIAN: 17.01.2018 */

/* ------- TODO ------- 
- Make sure that names are not ambigous. Maybe don't typedef default type and put prefix for structs?
- Binary serialization.
- Get rid of "tstring.h" dependency.
- Runtime reflection?
- Less painful way to get object type at runtime/compile time.
- Make sure perfomance is ok!
- Unions!
- Type/Property attributes!
- TEST IT! TEST IT! TEST IT!
- Get rid of sprintf & sscanf.

TEXT serialization format: 

Where: 
    - t: is a string
    - v: If value type is one of the defaults (char/bool/int/float) then actual value is written. Float for instance - (v:0.025). 
         Otherwise, object data is serialize as follows (t:typename s:size n:name v:def_type[]).


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
   char short_name[2];
   int health;
};

[Guy[short_name(2):ME][health:100]]

*/

/*
      Edits: 
	  - 12.08.2018: cr_type - user_data0 field added.
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
