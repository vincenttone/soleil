#include <assert.h>
#include "solByteCode.h"


void solByteCode_def(SolByteCode code,
					 const char* name,
					 const char* format,
					 const char* wide_format,
					 BasicType result_type,
					 int depth,
					 bool can_trap,
					 SolByteCode solVMCode)
{
	assert(wide_format == NULL || format != NULL,
		   "short form must exists when wide format exists");
	size_t len = format == NULL ? 0 : strlen(format);
	size_t wlen = wide_format == NULL ? 0 : strlen(wide_format);
}
