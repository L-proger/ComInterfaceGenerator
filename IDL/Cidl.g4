grammar Cidl;

module
    : //empty
    | (interfaceDefinition | structDefinition | enumDefinition)* EOF
    ;

attribute
    : LEFT_SQUARE_BRACKET local_type (LEFT_BRACKET argument_list RIGHT_BRACKET)? RIGHT_SQUARE_BRACKET
    ;

attribute_list
    : attribute ( attribute)*
    ;

interfaceDefinition 
    : attribute_list ID_INTERFACE local_type interfaceInheritanceList? LCURLY method+ RCURLY
    ;

structField
    : local_or_imported_type local_type SEMICOLON
    ;


structDefinition 
    : ID_STRUCT local_type LCURLY structField+ RCURLY
    ;

enumDefinition 
    : ID_ENUM local_type (COLON integer_primitive)? LCURLY enumField (',' enumField)* RCURLY
    ;

local_type
    : primitive
    | ID
    ;
imported_type_module
    : (ID '.')+
    ;

imported_type_name
    :  ID
    ;

imported_type
    : imported_type_module imported_type_name
    ;


local_or_imported_type
    : local_type
    | imported_type
    ;

reference_type_name
    : local_or_imported_type '&'
    ;

array_type
    : local_or_imported_type '[]'
    ;

method_name: local_type;

expression
    : STRING_LITERAL
    | numeric_literal
    ;

unary_expression
    : expression
    | MINUS unary_expression
    | PLUS unary_expression
    ;

MINUS
    : '-'
    ;

PLUS
    : '+'
    ;

argument
    : expression
    | unary_expression
    ;

argument_list
    : argument (',' argument)*
    ;

method_parameter
    : attribute_list? local_or_imported_type local_type
    | attribute_list? reference_type_name local_type
    | attribute_list? array_type local_type
    ;

method_parameter_list
    : method_parameter (',' method_parameter)*
    ;

method_return_type
    : array_type 
    | local_or_imported_type
    ;

method
    : method_return_type method_name LEFT_BRACKET method_parameter_list? RIGHT_BRACKET SEMICOLON
    ;

enumField 
    : local_type ('=' enum_field_value)?
    ;

interfaceInheritanceList
    : (COLON local_or_imported_type)
    ;

COMMENT
   : '/*' .*? '*/' -> channel (HIDDEN)
   ;


LINE_COMMENT
   : '//' ~ ('\n' | '\r')* '\r'? '\n' -> channel (HIDDEN)
   ;

numeric_literal
    : INTEGER_LITERAL
    | HEX_LITERAL
    | float_literal
    ;

enum_field_value 
    : MINUS? (INTEGER_LITERAL | HEX_LITERAL)
    ;

INTEGER_LITERAL
   : ('0' .. '9')+
   ;


HEX_LITERAL
   : '0' ('x' | 'X') HEX_DIGIT+
   ;


fragment HEX_DIGIT
   : ('0' .. '9' | 'a' .. 'f' | 'A' .. 'F')
   ;

primitive 
    : ID_INTEGER_PRIMITIVE
    | 'double'
    | 'float'
    | 'void'
    | 'IUnknown'
    | 'string'
    ;

integer_primitive 
    : ID_INTEGER_PRIMITIVE
    ;

ID_INTEGER_PRIMITIVE
    : ID_UINT8
    | ID_UINT16
    | ID_UINT32
    | ID_UINT64
    | ID_INT8
    | ID_INT16
    | ID_INT32
    | ID_INT64
    ;

ID_UINT8   : 'uint8' ;
ID_UINT16  : 'uint16' ;
ID_UINT32  : 'uint32' ;
ID_UINT64  : 'uint64' ;

ID_INT8   : 'int8' ;
ID_INT16  : 'int16' ;
ID_INT32  : 'int32' ;
ID_INT64  : 'int64' ;



ID_INTERFACE
    : 'interface'
    ;

ID_STRUCT
    : 'struct'
    ;

ID_ENUM
    : 'enum'
    ;

ID_IMPORT
    : 'import'
    ;

LEFT_SQUARE_BRACKET
   : '['
   ;

float_literal
    : FLOAT FLOAT_SUFFIX?
    ;

FLOAT
     :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT? 
     |   '.' ('0'..'9')+ EXPONENT? FLOAT_SUFFIX?
     |   ('0'..'9')+ EXPONENT FLOAT_SUFFIX?
     ;

FLOAT_SUFFIX
    : ('f' | 'F')
    ;

fragment EXPONENT 
    : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

fragment ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'"'|'\''|'\\')
    |   UNICODE_ESC
    |   OCTAL_ESC
    ;

fragment OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;
fragment UNICODE_ESC
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;


STRING_LITERAL
    :  '"' ( ESC_SEQ | ~('\\'|'"') )* '"' //{this->setText(this->getText().substr(1, this->getText().length()-2));}
    ;



RIGHT_SQUARE_BRACKET
   : ']'
   ;

ID  : ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

WS
   : (' ' | '\r' | '\t' | '\u000C' | '\n') -> channel (HIDDEN)
   ;

INTEGER : '0'..'9'+ ;

LEFT_BRACKET
   : '('
   ;


RIGHT_BRACKET
   : ')'
   ;


LCURLY  : '{'
 ;

RCURLY  : '}'
 ;

SEMICOLON
   : ';'
   ;


COLON
   : ':'
   ;