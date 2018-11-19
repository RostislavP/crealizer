#include "crealizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "crserialize.h"
#include "crdeserialize.h"

CR_ALLOC*   cr_alloc   = (CR_ALLOC*)   &malloc;
CR_REALLOC* cr_realloc = (CR_REALLOC*) &realloc;
CR_FREE*    cr_free    = (CR_FREE*)    &free;

cr_u32 cr_serialize_object(cr_byte8** stream, cr_u32 at, cr_byte8* object, CR_Type* type) {
	if(type->serialize_overload) {
		return(type->serialize_overload(stream, at, object, type));
	}

	return cr_write_object(stream, at, object, type);
}

cr_u32 cr_deserialize_object(cr_byte8* stream, cr_byte8** object, CR_Type* type) {
	if(type->deserialize_overload) {
		return(type->deserialize_overload(stream, object, type));
	}

	return cr_read_object(stream, object, type);
}

