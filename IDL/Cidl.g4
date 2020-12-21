grammar Cidl;

module
    : //empty
    | importExprList? (interfaceDefinition | structDefinition | enumDefinition)* EOF
    ;

attribute_guid
    : LEFT_SQUARE_BRACKET 'Guid' LEFT_BRACKET '"'  guid  '"'  RIGHT_BRACKET RIGHT_SQUARE_BRACKET
    ;

interfaceDefinition 
    : attribute_guid ID_INTERFACE identifier interfaceInheritanceList? LCURLY method+ RCURLY
    ;


structField 
    : (primitive | identifier) identifier SEMICOLON
    ;

structDefinition 
    : ID_STRUCT identifier LCURLY structField+ RCURLY
    ;

enumDefinition 
    : ID_ENUM identifier (COLON integer_primitive)? LCURLY enumField (',' enumField)* RCURLY
    ;

type_name: primitive | identifier;
method_name: identifier;

fragment GUIDH4
    : HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;

ID_GUID
    : (GUIDH4 GUIDH4 '-' GUIDH4 '-' GUIDH4 '-' GUIDH4 '-' GUIDH4 GUIDH4 GUIDH4)
    ;

guid
    : ID_GUID
    ;


attribute_ref
    : LEFT_SQUARE_BRACKET (ID_IN | ID_OUT | ID_INOUT) RIGHT_SQUARE_BRACKET
    ;

attribute_const
    : LEFT_SQUARE_BRACKET (ID_CONST) RIGHT_SQUARE_BRACKET
    ;



method_argument
    : attribute_const? type_name identifier
    | attribute_const? attribute_ref? type_name '*' identifier
    ;

method_argument_list
    : method_argument (',' method_argument)*
    ;

method
    : type_name method_name LEFT_BRACKET method_argument_list? RIGHT_BRACKET SEMICOLON
    ;

enumField 
    : identifier ('=' numeric_literal)?
    ;

interfaceInheritanceList
    : (COLON identifier)
    ;

importExpr
    : ID_IMPORT (identifier'.idl')
    ;

importExprList
    : importExpr+
    ;

COMMENT
   : '/*' .*? '*/' -> channel (HIDDEN)
   ;


LINE_COMMENT
   : '//' ~ ('\n' | '\r')* '\r'? '\n' -> channel (HIDDEN)
   ;

numeric_literal
    :INTEGER_LITERAL
    |HEX_LITERAL
    ;

INTEGER_LITERAL
   : ( '0' .. '9')+
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

ID_CONST
   : 'const'
   ;

ID_IN
   : 'in'
   ;

ID_OUT
   : 'out'
   ;

ID_INOUT
   : 'inout'
   ;

identifier
    : ID
    ;

LEFT_SQUARE_BRACKET
   : '['
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