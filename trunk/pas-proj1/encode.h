#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"
#include "symtab.h"

/*Function declarations*/
void declareVariable(ST_ID, TYPE);
int getAlignSize(TYPE);
int getSkipSize(TYPE);

#endif
