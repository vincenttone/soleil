#ifndef _SOL_REGEX_H_
#define _SOL_REGEX_H_ 1

typedef enum _SolRegexEncoding {
	SolRegexEncode_ascii = 1,
	SolRegexEncode_utf8 = 2,
} SolRegexEncoding;

typedef struct _SolRegexEngine {
	SolRegexEncoding e;
} SolRegexEngine;

typedef struct _SolRegex {
	
} SolRegex;

#endif
