#ifndef _SOL_REGEX_SYMBOL_ASCII_H_
#define _SOL_REGEX_SYMBOL_ASCII_H_ 1

#include "sol_pattern.h"

#define SOL_REGEX_SYMBOL_NUL 0 // \0

#define SOL_REGEX_SYMBOL_HT 9 // \t
#define SOL_REGEX_SYMBOL_EOL 10 // \n
#define SOL_REGEX_SYMBOL_VT 11 // \v
#define SOL_REGEX_SYMBOL_VT 12 // \f
#define SOL_REGEX_SYMBOL_CR 13 // \r

#define SOL_REGEX_SYMBOL_LBR 28 // (
#define SOL_REGEX_SYMBOL_RBR 29 // )

#define SOL_REGEX_SYMBOL_SPACE 32 // ' '
#define SOL_REGEX_SYMBOL_SHARP 35 // #

#define SOL_REGEX_SYMBOL_STAR 42 // *
#define SOL_REGEX_SYMBOL_PLUS 43 // +
#define SOL_REGEX_SYMBOL_COMMA 44 // ,
#define SOL_REGEX_SYMBOL_MIUS 45 // -
#define SOL_REGEX_SYMBOL_DOT 46 // .
#define SOL_REGEX_SYMBOL_SLASH 47 // /

#define SOL_REGEX_SYMBOL_N_0 48
#define SOL_REGEX_SYMBOL_N_9 57

#define SOL_REGEX_SYMBOL_COLON 58 // :
#define SOL_REGEX_SYMBOL_SEMICOLON 59 // ;

#define SOL_REGEX_SYMBOL_QM 63 // ?

#define SOL_REGEX_SYMBOL_L_A 65
#define SOL_REGEX_SYMBOL_L_Z 90

#define SOL_REGEX_SYMBOL_LSB 91 // [
#define SOL_REGEX_SYMBOL_BS 92  /* \ */
#define SOL_REGEX_SYMBOL_RSB 93 // ]

#define SOL_REGEX_SYMBOL_L_a 97
#define SOL_REGEX_SYMBOL_L_z 122

#define SOL_REGEX_SYMBOL_LBRACE 123 // {
#define SOL_REGEX_SYMBOL_VB 124 // |
#define SOL_REGEX_SYMBOL_RBRACE 125 // }

SolPattern* solRegexSymbolAscii_pattern();
int solRegexSymbolAscii_match(void*, void*);

#define solRegexSymbolAscii_char(s) (*(char*)s)

#endif
