#ifdef _SOL_REGISTER_GROUP_H_
#define _SOL_REGISTER_GROUP_H_ 1

#include "solRegister.h"

typedef struct _SolRegisterGroup_ {
	SolRegister *reg0;
	SolRegister *reg1;
	SolRegister *reg2;
	SolRegisterGroup *pre;
} SolRegisterGroup

#endif 
