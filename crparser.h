#ifndef _CR_PARSER_H_
#define _CR_PARSER_H_

/*

struct CR_STYPE Test [
    real32 a;
};
 */

#include "crcommon.h"

struct CR_Token {
	cr_byte8* at;
	cr_u32    length;
};

struct CR_Parser {
};

static void cr_read_types_from_source_file(cr_byte8* file_path, cr_byte8** out_stream) {
	FILE* file = fopen(file_path, "r");

	if(file) {
		cr_u32 size;

		// Read file size
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, size, SEEK_SET);

		cr_byte8* file_buffer = (cr_byte8*) cr_alloc(size);
		
		cr_byte8* cursor = file_buffer;

		while(cursor - file_buffer < size) {
			*cursor = fgetc(file); cursor++;
		}

		cursor = file_buffer;

		cr_free(file_buffer);
		fclose(file);
	}
}

#endif
