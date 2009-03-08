This directory contains the files comprising the skeleton of the Pascal
compiler "ppc3".

Makefile	- Compiler maintenance
README.txt	- This file
T1L80_err.pas	- Test file for 85% credit
T1L80_ok.pas	- Error-free test file for 85% credit
T1L90_err.pas	- Test file for 100% credit
T1L90_ok.pas	- Error-free test file for 100% credit
T1L100_err.pas	- Test file for 20% extra credit
T1L100_ok.pas	- Error-free test file for 20% extra credit
defs.h		- Global definitions
gram.y		- The skeleton grammar for Pascal
main.c		- The main routine
message.c	- Message handling routines
message.h	- Definitions for message.c
scan.l		- The specification of the lexical scanner
symtab.c	- Symbol table maintenance
symtab.h	- Definitions for symtab.c
types.c		- Processes Pascal type information
types.h		- Definitions for types.c
backend.c	- Routines for generating assembly code
backend.h	- Definitions for backend.c
utils.c		- Miscellaneous utilities
