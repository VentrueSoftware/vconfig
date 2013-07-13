/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 12-Jul-2013
 *    File: vc-lex.h
 * 
 *    Lexer/token definitions
 */

enum token_types {
    TOKEN_NULL,
    TOKEN_INVALID,
    TOKEN_LINE,
    TOKEN_COMMENT,
    TOKEN_EQUALS,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_IDENTIFIER,
    TOKEN_OPTPATH,
    TOKEN_BOOLEAN,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
};

typedef enum token_types TOKEN_TYPE;
