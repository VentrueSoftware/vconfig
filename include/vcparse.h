/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 06-Jun-2013
 *    File: parse.h
 * 
 * Recursive-descent parser for VConfig.
 *
 * VConfig EBNF:
 * 
 * upper = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" 
 *       | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" 
 *       | "U" | "V" | "W" | "X" | "Y" | "Z" ;
 * lower = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" 
 *       | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" 
 *       | "u" | "v" | "w" | "x" | "y" | "z" ;
 * alpha = upper | lower ;
 * numeric = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
 * 
 * newline = "\n" ;
 * whitespace = { " " | "\t" } ;
 * eol = { whitespace } , newline ;
 *
 * cfg = { newline | whitespace | comment | section | assignment } ;
 * 
 * comment = "#" , { any-char - newline } , newline ;
 * section = section-header , cfg , section-footer ;
 * assignment = identifier , whitespace , "=" , whitespace , value , eol ;
 * 
 * section-header = "[" , { whitespace } , section-name , { whitespace }, "]" , eol ;
 * section-footer = "[" , { whitespace } , "/" , section-name , { whitespace } , "]" , eol ;
 * section-name = identifier , { whitespace , identifier } ;
 * 
 * identifier = alpha, { alpha | numeric | "-" | "_" } ;
 * value = { string | number | boolean } ;
 * 
 * string = sstring | dstring ;
 * sstring = "'" , { any-char - "'" } , "'" ;
 * dstring = '"' , { any-char - '"' } , '"' ;
 * 
 * number = [ "-" ] , { numeric } ;
 * 
 * boolean = true | false ;
 * 
 * true = yes | ( ( "T" | "t" ) , [ ( "R" | "r" ) , ( "U" | "u" ) , ( "E" | "e" ) ] ) ;
 * yes = ( "Y" | "y" ) , [ ( "E" | "e" ) , ( "S" | "s" ) ] ;
 * 
 * false = no | ( ( "F" | "f" ) , [ ( "A" | "a" ) , ( "L" | "l" ) , ( "S" | "s" ) , ( "E" | "e" ) ] ) ;
 * no = ( "N" | "n" ) , [ "O" | "o" ] ;
 */

#ifndef __VCPARSE_H
#define __VCPARSE_H

/**********************************************************************/
/**** Begin Includes **************************************************/
/**********************************************************************/
#include <string.h>
#include "vctype.h"

/**********************************************************************/
/**** Begin Type Definitions ******************************************/
/**********************************************************************/

#define MAX_DEPTH 16   /* Nested sections are limited to 16 */

#define VC_TOKEN_TYPES(XX)                      \
    XX(INVALID)      /* Invalid token   */      \
    XX(NEWLINE)      /* New Line (\n)   */      \
    XX(SEMICOLON)    /* Semicolon (;)   */      \
    XX(SLASH)        /* Slash (/)       */      \
    XX(LBRACKET)     /* L. Bracket ([)  */      \
    XX(RBRACKET)     /* R. Bracket (])  */      \
    XX(COMMENT)      /* Comment         */      \
    XX(SECT_BEGIN)   /* Begin Section   */      \
    XX(SECT_END)     /* End Section     */      \
    XX(IDENTIFIER)   /* Identifier      */      \
    XX(ASSIGN)       /* Assignment (=)  */      \
    XX(BOOLEAN)      /* Boolean Value   */      \
    XX(INTEGER)      /* Numerical Value */      \
    XX(STRING)       /* String Value    */      

typedef enum {
    #define XX(name) VC_TOKEN_##name,
    VC_TOKEN_TYPES(XX)
    #undef XX
} vc_token_type;

typedef struct vc_token {
    vc_token_type type;
    char *position;
    size_t length;
} vc_token;

typedef struct vc_sect_token {
    char *position;
    size_t length;
    vc_sect *sect;
} vc_sect_token;

typedef struct vc_parser {
    char *file;    /* Name/path of file */
    char *ptr;  /* Location within the file */

    int line;   /* Current line within the file */
    int depth;  /* Current depth in the section stack. */
    
    /* Most recently read token */
    vc_token token;
    
    /* Section stack - keeps track of which section you're currently in */
    vc_sect_token sects[MAX_DEPTH + 1];
} vc_parser;

/**********************************************************************/
/**** Begin Definitions/Static Declarations ***************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Begin Function Prototypes ***************************************/
/**********************************************************************/
vc_sect *vc_parse_file(char *filename);
vc_sect *vc_parse_stream(char *buffer, vc_parser *parser);

#endif /* #ifndef __VCPARSE_H */
