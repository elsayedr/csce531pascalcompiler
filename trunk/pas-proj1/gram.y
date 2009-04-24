
/* Build 15
Change summary:
Edited production types to conform with Fenner's.  

Team Project:
Jeff Barton
Glenn Robertson
Jeremiah Shepherd
Josh Van Buren */

/* A Bison parser for the programming language Pascal.
  Copyright (C) 1989-2002 Free Software Foundation, Inc.

  Authors: Jukka Virtanen <jtv@hut.fi>
           Helsinki University of Technology
           Computing Centre
           Finland

           Peter Gerwinski <peter@gerwinski.de>
           Essen, Germany

           Bill Cox <bill@cygnus.com> (error recovery rules)

           Frank Heckenbach <frank@pascal.gnu.de>

  This file is part of GNU Pascal.

  GNU Pascal is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 1, or (at your
  option) any later version.

  GNU Pascal is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Pascal; see the file COPYING. If not, write to the
  Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA. */

/* Bison parser for ISO 7185 Pascal originally written on
 * 3 Feb 1987 by Jukka Virtanen <jtv@hut.fi>
 *
 * Modified for use at the University of South Carolina's CSCE 531
 * (Compiler Construction) course (Spring 2005) by Stephen Fenner
 * <fenner@cse.sc.edu>
 *
 * SHIFT/REDUCE CONFLICTS
 *
 * The dangling else will not cause a shift/reduce conflict - it's
 * solved by precedence rules.
 */

%{
#include "defs.h"
#include "tree.h"
#include "symtab.h"
#include "string.h"
#include "backend.h"
#include "encode.h"

/* Cause the `yydebug' variable to be defined. */
#define YYDEBUG 1

void set_yydebug(int);
void yyerror(const char *);

/*Stack variables*/
int base_offset_stack[BS_DEPTH];
int bo_top = -1;

/*Function id stack variables*/
ST_ID func_id_stack[BS_DEPTH];
int fi_top = -1;

int n; // used as temp variable for debugging

/* Like YYERROR but do call yyerror */
#define YYERROR1 { yyerror ("syntax error"); YYERROR; }

%}

/* Start symbol for the grammar */

%start pascal_program

/* The union representing a semantic stack entry */
%union {
    char * 	y_string;
    int		y_cint;
    long 	y_int;
    double 	y_real;
    TYPE	y_type;
    ID_LIST	y_idlist;
    ST_ID 	y_stid;
    PTR_OBJ	y_ptrobj;
    PARAM_LIST	y_params;
    MEMBER_LIST	y_member;
    INDEX_LIST 	y_indices;
    EXPR	y_expr;
    EXPR_LIST	y_exprlist;
    EXPR_NULLOP	y_nullop;
    EXPR_UNOP	y_unop;
    EXPR_BINOP	y_binop;
    EXPR_ID	y_exprid;
    FUNC_HEAD	y_funchead;
    DIRECTIVE	y_dir;
    NAME_OFFSET	y_nameoffset;
    VAL_LIST	y_valuelist;
    CASE_RECORD	y_caserec;
}

%token <y_string> LEX_ID

/* Reserved words. */

/* Reserved words in Standard Pascal */
%token LEX_ARRAY LEX_BEGIN LEX_CASE LEX_CONST LEX_DO LEX_DOWNTO LEX_END
%token LEX_FILE LEX_FOR LEX_FUNCTION LEX_GOTO LEX_IF LEX_LABEL LEX_NIL
%token LEX_OF LEX_PACKED LEX_PROCEDURE LEX_PROGRAM LEX_RECORD LEX_REPEAT
%token LEX_SET LEX_THEN LEX_TO LEX_TYPE LEX_UNTIL LEX_VAR LEX_WHILE LEX_WITH
%token LEX_FORWARD

/* The following ones are not tokens used in the parser.
 * However they are used in the same context as some tokens,
 * so assign unique numbers to them.
 */
%token pp_SIN pp_COS pp_EXP pp_LN pp_SQRT pp_ARCTAN rr_POW rr_EXPON
%token r_WRITE r_READ r_INITFDR r_LAZYTRYGET r_LAZYGET r_LAZYUNGET r_POW r_EXPON
%token z_ABS z_ARCTAN z_COS z_EXP z_LN z_SIN z_SQRT z_POW z_EXPON
%token set_card set_isempty set_equal set_le set_less set_in set_clear
%token set_include set_exclude set_include_range set_copy
%token set_intersection set_union set_diff set_symdiff
%token p_DONEFDR gpc_IOCHECK gpc_RUNTIME_ERROR

/* Redefinable identifiers. */

/* Redefinable identifiers in Standard Pascal */
%token <y_string> p_INPUT p_OUTPUT p_REWRITE p_RESET p_PUT p_GET p_WRITE p_READ
%token <y_string> p_WRITELN p_READLN p_PAGE p_NEW p_DISPOSE
%token <y_string> p_ABS p_SQR p_SIN p_COS p_EXP p_LN p_SQRT p_ARCTAN
%token <y_string> p_TRUNC p_ROUND p_PACK p_UNPACK p_ORD p_CHR p_SUCC p_PRED
%token <y_string> p_ODD p_EOF p_EOLN p_MAXINT p_TRUE p_FALSE

/* Additional redefinable identifiers for Borland Pascal */
%token bp_RANDOM bp_RANDOMIZE BREAK CONTINUE

/* redefinable keyword extensions */
%token RETURN_ RESULT EXIT FAIL p_CLOSE CONJUGATE p_DEFINESIZE SIZEOF
%token BITSIZEOF ALIGNOF TYPEOF gpc_RETURNADDRESS gpc_FRAMEADDRESS
%token LEX_LABEL_ADDR

/* GPC internal tokens */
%token <y_int> LEX_INTCONST
%token <y_string> LEX_STRCONST 
%token <y_real> LEX_REALCONST
%token LEX_RANGE LEX_ELLIPSIS

/* We don't declare precedences for operators etc. We don't need
   them since our rules define precedence implicitly, and too many
   precedences increase the chances of real conflicts going unnoticed. */
%token LEX_ASSIGN
%token '<' '=' '>' LEX_IN LEX_NE LEX_GE LEX_LE
%token '-' '+' LEX_OR LEX_OR_ELSE LEX_CEIL_PLUS LEX_CEIL_MINUS LEX_FLOOR_PLUS LEX_FLOOR_MINUS
%token '/' '*' LEX_DIV LEX_MOD LEX_AND LEX_AND_THEN LEX_SHL LEX_SHR LEX_XOR LEX_CEIL_MULT LEX_CEIL_DIV LEX_FLOOR_MULT LEX_FLOOR_DIV
%token LEX_POW LEX_POWER LEX_IS LEX_AS
%token LEX_NOT

/* Various extra tokens */
%token LEX_EXTERNAL ucsd_STR p_MARK p_RELEASE p_UPDATE p_GETTIMESTAMP p_UNBIND
%token p_EXTEND bp_APPEND p_BIND p_SEEKREAD p_SEEKWRITE p_SEEKUPDATE LEX_SYMDIFF
%token p_ARG p_CARD p_EMPTY p_POSITION p_LASTPOSITION p_LENGTH p_TRIM p_BINDING
%token p_DATE p_TIME LEX_RENAME LEX_IMPORT LEX_USES LEX_QUALIFIED LEX_ONLY

/* Precedence rules */

/* The following precedence declarations are just to avoid the dangling
   else shift-reduce conflict. We use prec_if rather than LEX_IF to
   avoid possible conflicts elsewhere involving LEX_IF going unnoticed. */
%nonassoc prec_if
%nonassoc LEX_ELSE

/* These tokens help avoid S/R conflicts from error recovery rules. */
%nonassoc lower_than_error
%nonassoc error

%type <y_expr> unsigned_number number constant constant_literal
%type <y_expr> expression actual_parameter static_expression
%type <y_expr> simple_expression term signed_primary primary factor
%type <y_expr> signed_factor variable_or_function_access predefined_literal
%type <y_expr> variable_or_function_access_no_as standard_functions
%type <y_expr> variable_or_function_access_no_standard_function
%type <y_expr> variable_or_function_access_no_id rest_of_statement
%type <y_expr> assignment_or_call_statement standard_procedure_statement
%type <y_expr> variable_access_or_typename optional_par_actual_parameter boolean_expression index_expression_item
%type <y_exprlist> actual_parameter_list optional_par_actual_parameter_list index_expression_list
%type <y_nullop> rts_fun_optpar
%type <y_unop> sign rts_fun_onepar rts_fun_parlist pointer_char
%type <y_binop> relational_operator multiplying_operator adding_operator
%type <y_exprid> variable_or_function_access_maybe_assignment
%type <y_idlist> id_list optional_par_id_list
%type <y_stid> identifier new_identifier label
%type <y_string> new_identifier_1 string combined_string 
%type <y_type> typename type_denoter type_denoter_1 new_ordinal_type new_pointer_type parameter_form
%type <y_type> new_structured_type subrange_type new_procedural_type open_array
%type <y_type> unpacked_structured_type array_type ordinal_index_type set_type file_type record_type functiontype
%type <y_indices> array_index_list
%type <y_ptrobj> pointer_domain_type
%type <y_params> optional_procedural_type_formal_parameter_list 
%type <y_params> procedural_type_formal_parameter_list
%type <y_params> procedural_type_formal_parameter
%type <y_params> optional_par_formal_parameter_list formal_parameter_list
%type <y_params> formal_parameter
%type <y_member> record_field_list fixed_part record_section variant_part
%type <y_funchead> function_heading
%type <y_dir> directive_list directive
%type <y_valuelist> case_constant_list one_case_constant
%type <y_caserec> case_element_list case_element
%type <y_string> simple_if if_statement case_statement conditional_statement simple_statement structured_statement
%type <y_cint> variable_declaration_part variable_declaration_list
%type <y_cint> variable_declaration simple_decl any_decl any_declaration_part function_declaration
%type <y_cint> repetitive_statement for_direction
%type <y_int> enumerator enumerated_type enum_list

%%

/* Pascal parser starts here */

pascal_program
    : /* Empty */
  {}| program_component_list
  {};

program_component_list
    : program_component
  {}| program_component_list program_component
  {};

program_component
    : main_program_declaration '.'
  {};

main_program_declaration
    : program_heading semi import_or_any_declaration_part { enter_main_body(); } statement_part	{ exit_main_body(); }
    ;

program_heading
    : LEX_PROGRAM new_identifier optional_par_id_list	{}  /* Ignore */
    ;

optional_par_id_list
    : /* Empty */		{}
    | '(' id_list ')'	{ $$ = $2; }
    ;

id_list
    : new_identifier			{ $$ = id_prepend(NULL,$1); }
    | id_list ',' new_identifier 	{ $$ = id_prepend($1,$3); }
    ;

typename
    : LEX_ID		{ $$ = check_typename(st_enter_id($1)); }
    ;

identifier
    : LEX_ID		{ $$ = (ST_ID)st_enter_id($1); if (debug) printf("ID: %s\n",$1); }
    ;

new_identifier
    : new_identifier_1	{ $$ = (ST_ID)st_enter_id($1); if (debug) printf("NEW ID: %s\n",$1); }
    ;

new_identifier_1
    : LEX_ID	/* Default */
    /* Standard Pascal constants */
    | p_MAXINT	{}
    | p_FALSE	{ $$ = "False"; }
    | p_TRUE	{ $$ = "True"; }
    /* Standard Pascal I/O */
    | p_INPUT
  {}| p_OUTPUT
  {}| p_REWRITE
  {}| p_RESET
  {}| p_PUT
  {}| p_GET
  {}| p_WRITE
  {}| p_READ
  {}| p_WRITELN
  {}| p_READLN
  {}| p_PAGE
  {}| p_EOF
  {}| p_EOLN	{}
    /* Standard Pascal heap handling */
    | p_NEW	{ $$ = "New"; }
    | p_DISPOSE	{ $$ = "Dispose"; }
    /* Standard Pascal arithmetic */
    | p_ABS
  {}| p_SQR
  {}| p_SIN
  {}| p_COS
  {}| p_EXP
  {}| p_LN
  {}| p_SQRT
  {}| p_ARCTAN
  {}| p_TRUNC
  {}| p_ROUND
    /* Standard Pascal transfer functions */
  {}| p_PACK	{}  /* Ignore */
    | p_UNPACK	{}  /* Ignore */
    /* Standard Pascal ordinal functions */
    | p_ORD	{ $$ = "Ord"; }
    | p_CHR	{ $$ = "Chr"; }
    | p_SUCC	{ $$ = "Succ"; }
    | p_PRED	{ $$ = "Pred"; }
    | p_ODD	{}
    /* Other extensions */
  {}| BREAK
  {}| CONTINUE
  {}| RETURN_
  {}| RESULT
  {}| EXIT
  {}| FAIL
  {}| SIZEOF
  {}| BITSIZEOF
  {};

/* Import or declaration part */  
  
import_or_any_declaration_part
    : any_declaration_import_part
  {};

any_declaration_import_part
    : /* Empty */
  {}| any_declaration_import_part any_or_import_decl
  {};

any_or_import_decl
    : import_part
  {}| any_decl
  {};

any_declaration_part
    : /* Empty */			{ $$ = 0; }
    | any_declaration_part any_decl	{ $$ = $1 + $2; }
    ;

any_decl
    : simple_decl		// default
    | function_declaration	// default
    ;

simple_decl
    : label_declaration_part	{}  /* Ignore */
    | constant_definition_part	{}  /* Ignore */
    | type_definition_part	{ $$ = 0; }
    | variable_declaration_part	// default
    ;

/* Label declaration part */

label_declaration_part
    : LEX_LABEL label_list semi	{}  /* Ignore */
    ;

label_list
    : label			{}  /* Ignore */
    | label_list ',' label	{}  /* Ignore */
    ;

label  /* Labels are returned as identifier nodes for compatibility with gcc */
	: LEX_INTCONST		{}  /* Ignore */
	| new_identifier	{}  /* Ignore */
	;

/* Constant definition part */

constant_definition_part
    : LEX_CONST constant_definition_list  {}  /* Ignore */
    ;

constant_definition_list
    : constant_definition				{}  /* Ignore */
    | constant_definition_list constant_definition	{}  /* Ignore */
    ;

constant_definition
    : new_identifier '=' static_expression semi  {}  /* Ignore */
    ;

constant
    : identifier		{}  /* Ignore */
    | sign identifier		{}  /* Ignore */
    | number			/* Default */
    | constant_literal	 	/* Default */
    ;

number
    : sign unsigned_number	{ $$ = sign_number($1, $2); }  
    | unsigned_number		/* Default */
    ;

unsigned_number
    : LEX_INTCONST	{ $$ = make_intconst_expr($1, ty_build_basic(TYSIGNEDLONGINT)); }
    | LEX_REALCONST	{ $$ = make_realconst_expr($1); }
    ;

sign
    : '+'	{ $$ = UPLUS_OP; }
    | '-'	{ $$ = NEG_OP; }
    ;

constant_literal
    : combined_string		{ $$ = make_strconst_expr($1); }
    | predefined_literal	{ $$ = $1; }
    ;

predefined_literal
    : LEX_NIL		{ $$ = make_null_expr(NIL_OP); }
    | p_FALSE		{ $$ = make_intconst_expr(0, ty_build_basic(TYSIGNEDCHAR)); }
    | p_TRUE		{ $$ = make_intconst_expr(1, ty_build_basic(TYSIGNEDCHAR)); }
    ;

combined_string
    : string 	/* Default */
    ;

string
    : LEX_STRCONST		/* Default */
    | string LEX_STRCONST	{}  		/* Ignore */
    ;

/* Type definition part */

type_definition_part
    : LEX_TYPE type_definition_list semi	{ resolve_ptr_types(); }
    ;

type_definition_list
    : type_definition				 {}  
    | type_definition_list semi type_definition  {}
    ;

type_definition		/* Installs a new identifier in the symtab as a new TYPENAME */	 
    : new_identifier '=' type_denoter 	{ make_type($1,$3); }  
    ;

type_denoter
    : typename		/* typename already a TYPE paramater */
    | type_denoter_1	/* Default */
    ;

type_denoter_1
    : new_ordinal_type
    | new_pointer_type
    | new_procedural_type	
    | new_structured_type	
    ;

new_ordinal_type
    : enumerated_type	{ $$ = ty_build_enum($1); }
    | subrange_type	/* Default */
    ;

enumerated_type
    : '(' enum_list ')'	 { $$ = $2; if (debug) printf("Enumerated list with %d elements\n",(int)$2); }
    ;

enum_list
    : enumerator		/* Default */		
    | enum_list ',' enumerator	{ $$ = $1 + $3; }
    ;

enumerator
    : new_identifier  { $$ = 1; }  /* Start count of enumerated type entries at 1 */
    ;

subrange_type				 
    : constant LEX_RANGE constant	{ $$ = make_subrange($1, $3); }  /* Builds the subrange type */
    ;

new_pointer_type
    : pointer_char pointer_domain_type	{ $$ = ty_build_ptr($2.id, $2.type); }
    ;

pointer_char
    : '^'  { $$ = DEREF_OP; }
    | '@'  { $$ = ADDRESS_OP; }
    ;

pointer_domain_type
    : new_identifier		{ $$.id = $1; $$.type = NULL; }
    | new_procedural_type	{ $$.id = NULL; $$.type = $1; }
    ;

new_procedural_type
    : LEX_PROCEDURE optional_procedural_type_formal_parameter_list		
	{ $$ = make_func($2, ty_build_basic(TYVOID)); }
    | LEX_FUNCTION optional_procedural_type_formal_parameter_list functiontype	
	{ $$ = make_func($2, $3); }
    ;

optional_procedural_type_formal_parameter_list
    : /* Empty */					{ $$ = NULL; }
    | '(' procedural_type_formal_parameter_list ')'	{ $$ = $2; }
    ;

procedural_type_formal_parameter_list
    : procedural_type_formal_parameter	
	{ $$ = $1; }
    | procedural_type_formal_parameter_list semi procedural_type_formal_parameter	
	{ $$ = param_concat($1, $3); }
    ;

procedural_type_formal_parameter
    : id_list				{ $$ = make_params($1, ty_build_basic(TYVOID), FALSE); id_list_free($1); }
    | id_list ':' typename		{ $$ = make_params($1, $3, FALSE); id_list_free($1); }
    | LEX_VAR id_list ':' typename	{ $$ = make_params($2, $4, TRUE); id_list_free($2); }
    | LEX_VAR id_list			{ $$ = make_params($2, ty_build_basic(TYVOID), TRUE); id_list_free($2); }
    ;

new_structured_type
    : LEX_PACKED unpacked_structured_type	{ $$ = $2;}	/* Ignore */
    | unpacked_structured_type			/* Default */
    ;

unpacked_structured_type
    : array_type
    | file_type		{}  // not configured 
    | set_type
    | record_type	
    ;

/* Array */

array_type
    : LEX_ARRAY '[' array_index_list ']' LEX_OF type_denoter	{ $$ = make_array($3, $6); }	
    ;

array_index_list
    : ordinal_index_type			{ $$ = index_append(NULL, $1); }
    | array_index_list ',' ordinal_index_type	{ $$ = index_append($1, $3); }
  ;

ordinal_index_type
    : new_ordinal_type	/* Default */
    | typename		/* Default */
    ;

/* File */

file_type
    : LEX_FILE direct_access_index_type LEX_OF type_denoter  {}
    ;

direct_access_index_type
    : /* Empty */
  {}| '[' ordinal_index_type ']'
  {};

/* Set */

set_type
    : LEX_SET LEX_OF type_denoter	{ $$ = ty_build_set($3);
					  if (debug) {
						printf("Built set of type: \n");
						ty_print_typetag(ty_query($3));
						printf("\n");
					  }
					}
    ;

/* Record */
	
record_type
    : LEX_RECORD record_field_list LEX_END	{ $$ = ty_build_struct($2);
					  	  if (debug) {
							printf("Created record with members:\n"); 
							ty_print_memlist($2);
							printf("\n");
						  }
						}
	;
						
record_field_list
    : /* Empty */				{ $$ = NULL; }
    | fixed_part optional_semicolon	/* Default */
    | fixed_part semi variant_part	// add combiner later
    | variant_part			{}
    ;

fixed_part
    : record_section			{ $$ = $1; }
    | fixed_part semi record_section	{ $$ = member_concat($1,$3); }
    ;

record_section
    : id_list ':' type_denoter  	{ $$ = make_members($1,$3); }
    ;

variant_part
    : LEX_CASE variant_selector LEX_OF variant_list rest_of_variant
  {};

rest_of_variant
    : optional_semicolon
  {}| case_default '(' record_field_list ')' optional_semicolon
  {};

variant_selector
    : new_identifier ':' variant_type
  {}| variant_type
  {};

variant_type
    : typename
  {}| new_ordinal_type
  {};

variant_list
    : variant
  {}| variant_list semi variant
  {};

variant
    : case_constant_list ':' '(' record_field_list ')'
  {};

case_constant_list
    : one_case_constant
  {}| case_constant_list ',' one_case_constant
  {};

one_case_constant
    : static_expression	{
			  TYPETAG cType;
			  long lo;
			  if(get_case_value($1, &lo, &cType) == TRUE)
			    new_case_value(cType, lo, lo);
			}
    | static_expression LEX_RANGE static_expression	{
							  TYPETAG cType1, cType2;
							  long lo, hi;
							  if(cType1 != cType2)
							    error("Range limits are of unequal type");
							  if(get_case_value($1, &lo, &cType1) == TRUE && get_case_value($3, &hi, &cType2))
							    new_case_value(cType1, lo, hi);
							}
    ;

/* Variable declaration part */

variable_declaration_part
    : LEX_VAR variable_declaration_list  { $$ = $2; resolve_ptr_types(); }
    ;

variable_declaration_list
    : variable_declaration				
    | variable_declaration_list variable_declaration	{ $$ = $1 + $2; }
    ;

variable_declaration
    : id_list ':' type_denoter semi  { 
					if (st_get_cur_block() <= 1)
					{
					  make_var($1,$3);
					  n = base_offset_stack[bo_top];
					  $$ = n;
					  if (debug) printf("Global - base offset stack top: %d\n", n);
					}
					else
					{
					  n = process_var_decl($1, $3, base_offset_stack[bo_top]);
				  	  $$ = n;
					  if (debug) printf("Local - base offset stack top: %d\n", n);
					}
					resolve_ptr_types(); 
				     }
    ;

/* Function declaration section */
    
function_declaration
    : function_heading semi directive_list semi	{ build_func_decl($1.id, $1.type, $3); }
    | function_heading semi 	{ $<y_string>$ = get_global_func_name($1.id); }	
				{ $<y_cint>$ = enter_function($1.id, $1.type, $<y_string>3); } 
        any_declaration_part 	{ if (debug) printf("Entering function body: %s\nAny decl part size = %d\n",$<y_string>3,$5);
				  enter_func_body($<y_string>3, $1.type, $5); } 
	statement_part semi	{ $$ = $<y_cint>4; exit_func_body($<y_string>3, $1.type); }
    ;

function_heading
    : LEX_PROCEDURE new_identifier optional_par_formal_parameter_list	{ $$.id = $2; $$.type = make_func($3, ty_build_basic(TYVOID)); }
    | LEX_FUNCTION new_identifier optional_par_formal_parameter_list functiontype	{ $$.id = $2; $$.type = make_func($3, $4); }
    ;

directive_list
    : directive				/*  Default */
    | directive_list semi directive	{}  /* Ignore */
    ;

directive
    : LEX_FORWARD	{ $$ = DIR_FORWARD; }
    | LEX_EXTERNAL	{ $$ = DIR_EXTERNAL; }
    ;

functiontype
    : /* Empty */	{ $$ = NULL; }  /* Empty function type */
    | ':' typename  	{ $$ = $2; }
    ;

/* Parameter specification section */

optional_par_formal_parameter_list
    : /* Empty */			{ $$ = NULL; }
    | '(' formal_parameter_list ')'	{ $$ = $2; }
    ;

formal_parameter_list
    : formal_parameter					/* Default */
    | formal_parameter_list semi formal_parameter	{ $$ = param_concat($1, $3); }
    ;

formal_parameter
    : id_list ':' parameter_form			{ $$ = make_params($1, $3, FALSE); }
    | LEX_VAR id_list ':' parameter_form		{ $$ = make_params($2, $4, TRUE); }
    | function_heading					{}  /* Ignore */
    | id_list ':' conformant_array_schema		{}  /* Ignore */
    | LEX_VAR id_list ':' conformant_array_schema	{}  /* Ignore */
    ;

parameter_form
    : typename		/* Default */
    | open_array 	/* Ignore */
    ;

conformant_array_schema
    : packed_conformant_array_schema	/* Ignore */
    | unpacked_conformant_array_schema	/* Ignore */
    ;

typename_or_conformant_array_schema
    : typename				/* Ignore */
    | packed_conformant_array_schema	/* Ignore */
    | unpacked_conformant_array_schema	/* Ignore */
    ;

packed_conformant_array_schema
    : LEX_PACKED LEX_ARRAY '[' index_type_specification ']' LEX_OF typename_or_conformant_array_schema	{}  /* Ignore */
    ;

unpacked_conformant_array_schema
    : LEX_ARRAY '[' index_type_specification_list ']' LEX_OF typename_or_conformant_array_schema	{}  /* Ignore */
    ;

index_type_specification
    : new_identifier LEX_RANGE new_identifier ':' typename  {}	// ignore
    ;

index_type_specification_list
    : index_type_specification
  {}| index_type_specification_list semi index_type_specification
  {};

open_array
    : LEX_ARRAY LEX_OF typename		{ $$ = ty_build_ptr(NULL, $3); }	/* Ignore */
    ;

statement_part
    : compound_statement
  {};

compound_statement
    : LEX_BEGIN statement_sequence LEX_END
  {};

statement_sequence
    : statement
  {}| statement_sequence semi statement
  {};

statement
    : label ':' unlabelled_statement
  {}| unlabelled_statement	// default
    ;

unlabelled_statement
    : structured_statement
  {}| simple_statement		// default
    ;

structured_statement
    : compound_statement	{}
    | with_statement	{}
    | conditional_statement	{}
    | { char * newSymb = new_symbol(); pushEndLabel(newSymb); } repetitive_statement	{ $$ = popEndLabel(); }
    ;

with_statement
    : LEX_WITH structured_variable_list LEX_DO statement
  {};

structured_variable_list
    : structured_variable
  {}| structured_variable_list ',' structured_variable
  {};

structured_variable
    : variable_or_function_access
  {};

conditional_statement
    : if_statement
  {}| case_statement
  {};

simple_if
    : LEX_IF boolean_expression {
				  /*Checks the expression*/
				  if(checkBoolean($2) == TRUE)
				  {
				    char* startIf = ifInit($<y_expr>2);
				    $$ = startIf;
				  }
	
				} LEX_THEN statement	{ $$ = $<y_string>3; }
    ;

if_statement
    : simple_if LEX_ELSE { $$ = ifClose($<y_string>1);} statement { b_label($<y_string>3); }
    | simple_if %prec prec_if { b_label($<y_string>1); }
    ;

case_statement
    : LEX_CASE expression LEX_OF { 
				    if(is_lval($2) == FALSE)
				      encode_expr($2);
				    else
				      encode_expr(make_un_expr(DEREF_OP, $2));
				    VAL_LIST newList = malloc(sizeof(VAL_LIST_REC));  
				    $<y_valuelist>$ = newList; 
				 } 
				case_element_list optional_semicolon_or_else_branch LEX_END	{}
    ;

optional_semicolon_or_else_branch
    : optional_semicolon
  {}| case_default statement_sequence
  {};

case_element_list
    : case_element	{ $$ = $1; }
    | case_element_list semi { $<y_caserec>$ = $1; } case_element	{
									  /*Checks for case duplicates*/
									}
    ;

case_element
    : case_constant_list ':' { encode_dispatch($1, new_symbol()); } statement	{}
    ;

case_default
    : LEX_ELSE
  {}| semi LEX_ELSE
  {};

repetitive_statement
    : repeat_statement
  {}| while_statement
  {}| for_statement
  {};

repeat_statement
    : LEX_REPEAT statement_sequence LEX_UNTIL boolean_expression
  {};

while_statement
    : LEX_WHILE boolean_expression LEX_DO {
					    if(checkBoolean($2) == TRUE)
					    {
					      char* startWhile;
					      startWhile = whileInit($<y_expr>2);
					      $<y_string>$ = startWhile;
					    }
					  }
					  {
					    char* endWhile;
					    endWhile = whileCond();
					    $<y_string>$ = endWhile;
					  }
	statement
	{					 					 
	  whileLoop($<y_string>4,$<y_string>5);
	}
	;

for_statement
    : LEX_FOR variable_or_function_access LEX_ASSIGN expression for_direction expression LEX_DO { BOOLEAN check = check_for_preamble($2, $4, $6); $<y_string>$ = encode_for_preamble($2, $4, $5, $6); } statement	{if($5 == 0) b_inc_dec(TYSIGNEDLONGINT, B_PRE_INC,1); else b_inc_dec(TYSIGNEDLONGINT, B_PRE_DEC, 1); }
    ;

for_direction
    : LEX_TO	{ $$ = 0; }
    | LEX_DOWNTO	{ $$ = 1; }	
    ;

simple_statement
    : empty_statement	{}
    | goto_statement	{}
    | assignment_or_call_statement	{ encode_expr($1); }
    | standard_procedure_statement	{ encode_expr($1); }
    | statement_extensions	{ $<y_string>$ = peekEndLabel(); }
    ;

empty_statement
    : /* Empty */ %prec lower_than_error
  {};

goto_statement
    : LEX_GOTO label
  {};

/* Function calls */

optional_par_actual_parameter_list
    : /* Empty */			{ $$ = NULL; }
    | '(' actual_parameter_list ')'	{ $$ = expr_list_reverse($2); }
    ;

actual_parameter_list
    : actual_parameter					{ $$ = expr_prepend(NULL,$1); }
    | actual_parameter_list ',' actual_parameter	{ $$ = expr_prepend($1,$3); }
    ;

actual_parameter
    : expression	/* Default */
    ;

/* Assignment and procedure calls */

assignment_or_call_statement
    : variable_or_function_access_maybe_assignment rest_of_statement	{ $$ = check_assign_or_proc_call($1.expr,$1.id,$2); }	
    ;

variable_or_function_access_maybe_assignment
    : identifier					{ $$.expr = make_id_expr($1); $$.id = $1; }
    | address_operator variable_or_function_access	{}	/* Ignore */
    | variable_or_function_access_no_id			{ $$.expr = $1; $$.id = NULL; }		
    ;

rest_of_statement
    : /* Empty */			{ $$ = NULL; }
    | LEX_ASSIGN expression		{ $$ = $2; }	
    ;

standard_procedure_statement
    : rts_proc_onepar '(' actual_parameter ')'
  {}| rts_proc_parlist '(' actual_parameter_list ')'
  {}| p_WRITE optional_par_write_parameter_list
  {}| p_WRITELN optional_par_write_parameter_list
  {}| p_READ optional_par_actual_parameter_list
  {}| p_READLN optional_par_actual_parameter_list
  {}| p_PAGE optional_par_actual_parameter_list
  {}| ucsd_STR '(' write_actual_parameter_list ')'
  {}| p_DISPOSE '(' actual_parameter ')'				{ $$ = make_un_expr(DISPOSE_OP, $3); }
    | p_DISPOSE '(' actual_parameter ',' actual_parameter_list ')'	{ $$ = make_un_expr(DISPOSE_OP, $3); }
    ;

optional_par_write_parameter_list
    : /* Empty */
  {}| '(' write_actual_parameter_list ')'
  {};

write_actual_parameter_list
    : write_actual_parameter
  {}| write_actual_parameter_list ',' write_actual_parameter
  {};

write_actual_parameter
    : actual_parameter
  {}| actual_parameter ':' expression
  {}| actual_parameter ':' expression ':' expression
  {};

/* Run time system calls with one parameter */
rts_proc_onepar
    : p_PUT
  {}| p_GET
  {}| p_MARK
  {}| p_RELEASE
  {}| p_CLOSE
  {}| p_UPDATE
  {}| p_GETTIMESTAMP
  {}| p_UNBIND
  {};

rts_proc_parlist
    : p_REWRITE     /* Up to three args */
  {}| p_RESET       /* Up to three args */
  {}| p_EXTEND      /* Up to three args */
  {}| bp_APPEND     /* Up to three args */
  {}| p_PACK        /* Three args */
  {}| p_UNPACK      /* Three args */
  {}| p_BIND        /* Two args */
  {}| p_SEEKREAD
  {}| p_SEEKWRITE
  {}| p_SEEKUPDATE
  {}| p_DEFINESIZE  /* Two args */
  {}| LEX_AND       /* Two args */
  {}| LEX_OR        /* Two args */
  {}| LEX_NOT       /* One arg */
  {}| LEX_XOR       /* Two args */
  {}| LEX_SHL       /* Two args */
  {}| LEX_SHR       /* Two args */
  {};

statement_extensions
    : return_statement
  {}| continue_statement
  {}| break_statement	{ if(is_exit_label() == FALSE) error("Break statement not inside loop"); }
    ;

return_statement
    : RETURN_
  {}| RETURN_ expression
  {}| EXIT
  {}| FAIL
  {};

break_statement
    : BREAK
  {};

continue_statement
    : CONTINUE
  {};

variable_access_or_typename
    : variable_or_function_access_no_id		// default	
    | LEX_ID					{ $$ = make_id_expr(st_enter_id($1)); if (debug) printf("ID: %s\n",$1); }
    ;

index_expression_list
    : index_expression_item	{ $$ = expr_prepend(NULL, $1); }
    | index_expression_list ',' index_expression_item	{ $$ = expr_prepend($1, $3); };

index_expression_item	
    : expression	/*Default*/
    | expression LEX_RANGE expression	{$$ = $1;}

/* Expressions */

static_expression
    : expression	/* Default */
    ;

boolean_expression
    : expression	/* Default */
    ;

expression  
    : expression relational_operator simple_expression	{ $$ = make_bin_expr($2,$1,$3); }
    | expression LEX_IN simple_expression		{}	// what do we do with IN operator?
    | simple_expression					/* Default */
    ;

simple_expression
    : term					/* Default */
    | simple_expression adding_operator term	{ $$ = make_bin_expr($2,$1,$3); }
    | simple_expression LEX_SYMDIFF term	{ $$ = make_bin_expr(SYMDIFF_OP,$1,$3); }
    | simple_expression LEX_OR term		{ $$ = make_bin_expr(OR_OP,$1,$3); }
    | simple_expression LEX_XOR term		{ $$ = make_bin_expr(XOR_OP,$1,$3); }
    ;

term
    : signed_primary				// default
    | term multiplying_operator signed_primary	{ $$ = make_bin_expr($2,$1,$3); }
    | term LEX_AND signed_primary		{ $$ = make_bin_expr(AND_OP,$1,$3); }
    ;

signed_primary
    : primary			// default
    | sign signed_primary	{ $$ = make_un_expr($1, $2); }
    ;

primary
    : factor			// default
    | primary LEX_POW factor
  {}| primary LEX_POWER factor
  {}| primary LEX_IS typename
  {};

signed_factor
    : factor			// default
    | sign signed_factor	{ $$ = make_un_expr($1, $2); }
    ;

factor
    : variable_or_function_access	{ if (ty_query($1->type)==TYFUNC) $$=make_fcall_expr($1, NULL);
					  else $$ = $1; }
    | constant_literal			// default
    | unsigned_number			// default
    | set_constructor			{}
    | LEX_NOT signed_factor		{}
    | address_operator factor		{}
    ;

address_operator
    : '@'
  {};

variable_or_function_access
    : variable_or_function_access_no_as			// default
    | variable_or_function_access LEX_AS typename	{}
    ;

variable_or_function_access_no_as
    : variable_or_function_access_no_standard_function	// default
    | standard_functions				// default
    ;

variable_or_function_access_no_standard_function
    : identifier					{ $$ = make_id_expr($1); }
    | variable_or_function_access_no_id			// default
    ;

variable_or_function_access_no_id
    : p_OUTPUT							{}
    | p_INPUT							{}
    | variable_or_function_access_no_as '.' new_identifier	{}
    | '(' expression ')'					{ $$ = $2; }
    | variable_or_function_access pointer_char			{ $$ = make_un_expr(INDIR_OP,$1); }
    | variable_or_function_access '[' index_expression_list ']'	{ $$ = make_array_access_expr($1, $3); }
    | variable_or_function_access_no_standard_function '(' actual_parameter_list ')'	{ $$ = make_fcall_expr($1, expr_list_reverse($3)); } 
    | p_NEW '(' variable_access_or_typename ')'			{ $$ = make_un_expr(NEW_OP, $3); }
    ;

set_constructor
    : '[' ']'
  {}| '[' set_constructor_element_list ']'
  {};

set_constructor_element_list
    : member_designator
  {}| set_constructor_element_list ',' member_designator
  {};

member_designator
    : expression
  {}| expression LEX_RANGE expression
  {};

standard_functions
    : rts_fun_onepar '(' actual_parameter ')'		{ $$ = make_un_expr($1,$3); }
    | rts_fun_optpar optional_par_actual_parameter	{ $$ = make_un_expr($1,$2); }
    | rts_fun_parlist '(' actual_parameter_list ')'	{ $$ = make_un_expr($1,$3->expr); }
    ;

optional_par_actual_parameter
    : /* Empty */			{ $$ = NULL; }
    | '(' actual_parameter ')'	{ $$ = $2; }	
    ;

rts_fun_optpar
    : p_EOF	{ $$ = UN_EOF_OP; }
    | p_EOLN	{ $$ = UN_EOLN_OP; }
    ;

rts_fun_onepar
    : p_ABS  		{ $$ = ABS_OP; }
    | p_SQR  		{ $$ = SQR_OP; }
    | p_SIN		{ $$ = SIN_OP; }
    | p_COS		{ $$ = COS_OP; }
    | p_EXP		{ $$ = EXP_OP; }
    | p_LN		{ $$ = LN_OP; }
    | p_SQRT		{ $$ = SQRT_OP; }
    | p_ARCTAN		{ $$ = ARCTAN_OP; }
    | p_ARG		{ $$ = ARG_OP; }
    | p_TRUNC		{ $$ = TRUNC_OP; }
    | p_ROUND		{ $$ = ROUND_OP; }
    | p_CARD		{ $$ = CARD_OP; }
    | p_ORD		{ $$ = ORD_OP; }
    | p_CHR		{ $$ = CHR_OP; }
    | p_ODD		{ $$ = ODD_OP; }
    | p_EMPTY		{ $$ = EMPTY_OP; }
    | p_POSITION	{ $$ = POSITION_OP; }
    | p_LASTPOSITION	{ $$ = LASTPOSITION_OP; }
    | p_LENGTH		{ $$ = LENGTH_OP; }
    | p_TRIM		{ $$ = TRIM_OP; }
    | p_BINDING		{ $$ = BINDING_OP; }
    | p_DATE		{ $$ = DATE_OP; }
    | p_TIME  		{ $$ = TIME_OP; }
    ;

rts_fun_parlist
    : p_SUCC	{ $$ = UN_SUCC_OP; } /* -or- $$ = BIN_SUCC_OP; */   /* One or two args */
    | p_PRED	{ $$ = UN_PRED_OP; } /* -or- $$ = BIN_PRED_OP; */   /* One or two args */
    ;

relational_operator
    : LEX_NE	{ $$ = NE_OP; }
    | LEX_LE	{ $$ = LE_OP; }
    | LEX_GE	{ $$ = GE_OP; }
    | '='	{ $$ = EQ_OP; }
    | '<'	{ $$ = LESS_OP; }
    | '>'	{ $$ = GREATER_OP; }
    ;

multiplying_operator
    : LEX_DIV	{ $$ = REALDIV_OP; }
    | LEX_MOD	{ $$ = MOD_OP; }
    | '/'	{ $$ = DIV_OP; }
    | '*'	{ $$ = MUL_OP; }
    ;

adding_operator
    : '-'	{ $$ = SUB_OP; }
    | '+'	{ $$ = ADD_OP; }
    ;

semi
    : ';'	{}
    ;

optional_semicolon
    : /* Empty */	{}
    | ';'	{}
    ;

optional_rename
    : /* Empty */
  {}| LEX_RENAME new_identifier
  {};

import_part
    : LEX_IMPORT import_specification_list semi
  {}| LEX_USES uses_list semi
  {};

import_specification_list
    : import_specification
  {}| import_specification_list semi import_specification
  {};

uses_list
    : import_specification
  {}| uses_list ',' import_specification
  {};

import_specification
    : new_identifier optional_access_qualifier optional_import_qualifier optional_unit_filename
  {};

optional_access_qualifier
    : /* Empty */
  {}| LEX_QUALIFIED
  {};

optional_import_qualifier
    : /* Empty */
  {}| '(' import_clause_list ')'
  {}| LEX_ONLY '(' import_clause_list ')'
  {};

optional_unit_filename
    : /* Empty */
  {}| LEX_IN combined_string
  {};

import_clause_list
    : import_clause
  {}| import_clause_list ',' import_clause
  {};

import_clause
    : new_identifier optional_rename
  {};

%%

void yyerror(const char *msg)
{
    error(msg);
}

/* Sets the value of the 'yydebug' variable to VALUE.
   This is a function so we don't have to have YYDEBUG defined
   in order to build the compiler.  */
void
set_yydebug (value)
     int value;
{
#if YYDEBUG != 0
  yydebug = value;
#else
  warning ("YYDEBUG not defined.");
#endif
}
