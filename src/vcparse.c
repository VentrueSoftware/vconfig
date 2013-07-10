/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 06-Jun-2013
 *    File: parse.c
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
 * identifier = alpha, { alpha | numeric | "-" | "_" | "/" | "\" } ;
 * value = { string | integer | float | boolean } ;
 * 
 * string = sstring | dstring ;
 * sstring = "'" , { any-char - "'" } , "'" ;
 * dstring = '"' , { any-char - '"' } , '"' ;
 * 
 * integer = [ "-" ] , numeric, { numeric } ;
 * 
 * float = [ "-" ] , { numeric } , "." , { numeric } ;
 * 
 * boolean = true | false ;
 * 
 * true = yes | ( ( "T" | "t" ) , [ ( "R" | "r" ) , ( "U" | "u" ) , ( "E" | "e" ) ] ) ;
 * yes = ( "Y" | "y" ) , [ ( "E" | "e" ) , ( "S" | "s" ) ] ;
 * 
 * false = no | ( ( "F" | "f" ) , [ ( "A" | "a" ) , ( "L" | "l" ) , ( "S" | "s" ) , ( "E" | "e" ) ] ) ;
 * no = ( "N" | "n" ) , [ "O" | "o" ] ;
 */

/**********************************************************************/
/**** Includes ********************************************************/
/**********************************************************************/
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vcparse.h"
#include "vcerror.h"

/**********************************************************************/
/**** Macro Definitions ***********************************************/
/**********************************************************************/



#define SKIP_WHITESPACE(ptr)                        \
    while (*(ptr) == ' ' || *(ptr) == '\t') (ptr)++;

#define VC_THROW_ERROR(type, ctx, ...) {                \
    vc_print_error(VC_ERROR_##type, ctx, ##__VA_ARGS__);\
    goto err;                                           \
}

#define SINGLE_CHAR_TOKEN(c, t)                 \
    c:                                          \
        token->type = VC_TOKEN_##t;             \
        token->length = 1;                      \
        if (*(parser->ptr)) parser->ptr += 1;

#define ACCEPT(t) if (parser->token.type == VC_TOKEN_##t)
#define EXPECT(t) if (parser->token.type != VC_TOKEN_##t) {                                 \
    VC_THROW_ERROR(EXPECTED, parser, vc_token_str[VC_TOKEN_##t], vc_token_str[parser->token.type]); \
} else

#define REQUIRE(x)\
    if (!x) {\
        VC_THROW_ERROR(UNEXPECTED_EOF, parser);\
        goto err;\
    }


static const char *vc_token_str[] = {
    #define XX(name) "VC_TOKEN_" #name,
    VC_TOKEN_TYPES(XX)
    #undef XX
};

#define DEF_PARSE_RULE(rule) static int vc_parse_##rule(vc_parser *parser)
#define PARSE(rule) if (!vc_parse_##rule(parser)) {\
    fprintf(stderr, "Parse failure in rule: '%s'\n", #rule);\
    goto err;\
}


/**********************************************************************/
/**** Static Function Prototypes **************************************/
/**********************************************************************/

/* Initializes the parser */
static void vc_parser_init(vc_parser *parser, char *data);

/* Obtain the next token from the parser */
static int vc_parser_get_token(vc_parser *parser);

/* Character validators */
static unsigned char is_boolean(char *str, size_t length, int *boolval);
static inline unsigned char is_identifier_char(char c);
static inline unsigned char is_alpha_char(char c);
static inline unsigned char is_digit_char(char c);
static inline unsigned char is_id_symbol_char(char c);

/* Parse subrules */
DEF_PARSE_RULE(assignment);
DEF_PARSE_RULE(section);

/**********************************************************************/
/**** Function Definitions ********************************************/
/**********************************************************************/

/**********************************************************************/
/******** API Function Definitions ************************************/
/**********************************************************************/

vc_sect *vc_parse_file(char *filename) {
    int fd;
    size_t fsize;
    char *buffer;
    vc_parser parser_inst;
    vc_sect *conf;
    
    if ((fd = open(filename, O_RDONLY)) < 0) {
        VC_THROW_ERROR(FILE, 0, filename);
    }
    
    /* Determine size of file */
    fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    
    /* Allocate a buffer to hold the file's contents.  Right now, we
     * do single-chunk reads, as streaming isn't implemented yet. */
    buffer = (char *)malloc(sizeof(char) * (fsize + 1));
    bzero(buffer, sizeof(char) * (fsize + 1));
    vc_parser_init(&parser_inst, buffer);
    parser_inst.file = filename;
    
    /* Read the file, and parse the contents. */
    read(fd, buffer, fsize);
    conf = vc_parse_stream(buffer, &parser_inst);
    close(fd);
    
    free(buffer);
    
    return conf;
    
err:
    return 0;
}

vc_sect *vc_parse_stream(char *buffer, vc_parser *parser) {
    parser->ptr = buffer;
    
    /* Loop until we have no more tokens to parse, which indicates EOF */
    while (vc_parser_get_token(parser)) {
        
        /* If it's an invalid token, throw an error. */
        ACCEPT(INVALID) {
            VC_THROW_ERROR(INVALID_TOKEN, parser, parser->token.length, parser->token.position);
        }
        
        /* LBRACKET ([) at this point indicates that a section start/end
         * will be coming up. */
        else ACCEPT(LBRACKET) {
            PARSE(section);
        } 
        
        /* If an identifier is parsed, then we assume assignment.  In
         * the future, we will need to check directives before assuming
         * assignment is taking place. */
        else ACCEPT(IDENTIFIER) {
            PARSE(assignment);
        } 
        
        /* Whitespace stuff */
        else ACCEPT(NEWLINE); 
        else ACCEPT(COMMENT);
        else ACCEPT(SEMICOLON);
        
        /* Unexpected token type */
        else {
            VC_THROW_ERROR(UNEXPECTED, parser, 
                vc_token_str[parser->token.type], parser->token.length, 
                parser->token.position
            );
        }
    }
    
    /* By the end of the file, we should be back at depth zero.  If not,
     * then some sections were not closed before the file ended. */
    if (parser->depth) {
        VC_THROW_ERROR(NONZERO_DEPTH, parser, parser->depth);
    }
    
    /* Return the root section */
    return parser->sects[0].sect;

err:
    /* Clean up the section(s) created */
    vc_sect_destroy(parser->sects[0].sect);
    return 0;
}


/**********************************************************************/
/******** Static Function Definitions *********************************/
/**********************************************************************/

/**********************************************************************/
/************ Parse Subrules ******************************************/
/**********************************************************************/
DEF_PARSE_RULE(assignment) {
    /* Save the optname and optlength */
    char *optname = parser->token.position;
    size_t optlength = parser->token.length;
    
    /* Get the next token and expect it is an assignment token */
    REQUIRE(vc_parser_get_token(parser));

    EXPECT(ASSIGN) {
        REQUIRE(vc_parser_get_token(parser));

        /* Handle an integer, by copying into a buffer and invoking atoi() */
        ACCEPT(INTEGER) {
            char buffer[32];
            int *v = (int *)malloc(sizeof(int));
            strncpy(buffer, parser->token.position, parser->token.length);
            buffer[parser->token.length] = '\0';
            *v = atoi(buffer);
            vc_addoptn(parser->sects[parser->depth].sect, optname, optlength, VC_INTEGER, v);
            return 1;
        }
        
        /* Handle a float, by copying into a buffer and invoking atof() */
        else ACCEPT(FLOAT) {
            char buffer[32];
            double *v = (double *)malloc(sizeof(double));
            strncpy(buffer, parser->token.position, parser->token.length);
            buffer[parser->token.length] = '\0';
            *v = atof(buffer);
            vc_addoptn(parser->sects[parser->depth].sect, optname, optlength, VC_FLOAT, v);
            return 1;
        }
        
        /* Accept a string by essentially strduping */
        else ACCEPT(STRING) {
            char *v = (char *)malloc(sizeof(char) * (parser->token.length + 1));
            strncpy(v, parser->token.position, parser->token.length);
            v[parser->token.length] = '\0';
            vc_addoptn(parser->sects[parser->depth].sect, optname, optlength, VC_STRING, v);
            return 1;
        }
        
        /* Handle a boolean as an integer (1 or 0) */
        else ACCEPT(BOOLEAN) {
            int *v = (int *)malloc(sizeof(int));
            *v = parser->token.length;
            vc_addoptn(parser->sects[parser->depth].sect, optname, optlength, VC_BOOLEAN, v);
            return 1;
        } 
        
        /* Else, we got something invalid.  Error. */
        else VC_THROW_ERROR(UNEXPECTED, parser, vc_token_str[parser->token.type], parser->token.length, parser->token.position);
    }
err:
    return 0;
}

DEF_PARSE_RULE(section) {
    vc_token tok;
    tok.type = VC_TOKEN_SECT_BEGIN;
    
    /* Check for section end, which starts with a '/' */
    if(*(parser->ptr) == '/') {
        tok.type = VC_TOKEN_SECT_END;
        (parser->ptr)++;
    }
    
    /* Get the identifier, which will be the section name */
    REQUIRE(vc_parser_get_token(parser));    
    
    EXPECT(IDENTIFIER) {
        tok.position = parser->token.position;
        tok.length = parser->token.length;
        
        /* Expect a RBRACKET (]) for closing the section */
        REQUIRE(vc_parser_get_token(parser));        
        EXPECT(RBRACKET) {
            
            /* If we're starting a new section, allocate a new vc_sect */
            if (tok.type == VC_TOKEN_SECT_BEGIN) {
                /* Don't allow depth overflow */
                if (parser->depth == MAX_DEPTH) VC_THROW_ERROR(DEPTH_OVERFLOW, parser, MAX_DEPTH);
                char *label = strndup(tok.position, tok.length + 1);
                vc_opt *newsect_opt;
                label[tok.length] = '\0';
                newsect_opt = vc_addopt(parser->sects[parser->depth].sect, label, VC_SECTION, 0);
                parser->depth++;
                parser->sects[parser->depth].position = tok.position;
                parser->sects[parser->depth].length = tok.length;
                parser->sects[parser->depth].sect = (vc_sect *)newsect_opt->value;
                free(label);
            } else {
                /* Otherwise, verify we're closing the most recently-opened section.
                 * Don't allow depth underflow */
                if (parser->depth == 0) VC_THROW_ERROR(DEPTH_UNDERFLOW, parser);
                if (strncmp(tok.position, parser->sects[parser->depth].position, tok.length)) {
                    VC_THROW_ERROR(SECT_MISMATCH, parser,
                        parser->sects[parser->depth].length, 
                        parser->sects[parser->depth].position,
                        tok.length, tok.position
                    );
                }
                parser->depth--;
            }
            return 1;
        }
    }
err:    
    return 0;
}

/**********************************************************************/
/************ Helper functions ****************************************/
/**********************************************************************/

static void vc_parser_init(vc_parser *parser, char *data) {
    parser->ptr = data;     /* Initialize pointer to beginning of data */
    parser->line = 1;       /* Initialize line counter to one */
    parser->depth = 0;      /* Initialize section depth to zero */
    
    parser->sects[0].position = "root";
    parser->sects[0].length = 4;
    parser->sects[0].sect = vc_root_sect();
}

static int vc_parser_get_token(vc_parser *parser) {
    vc_token *token;
    #define PPTR (parser->ptr)
    if (!parser || !*PPTR) return 0;
    token = &(parser->token);
    
    /* Skip all whitespace */
    SKIP_WHITESPACE(parser->ptr);
    token->position = parser->ptr;
    
    /* Determine type of token */
    switch (*PPTR) {
        /* Test single-character tokens */
        case SINGLE_CHAR_TOKEN('\n', NEWLINE); parser->line++; break;
        case SINGLE_CHAR_TOKEN(';', SEMICOLON); break;
        case SINGLE_CHAR_TOKEN('[', LBRACKET);  break;
        case SINGLE_CHAR_TOKEN(']', RBRACKET);  break;
        case SINGLE_CHAR_TOKEN('=', ASSIGN);    break;
        case '#':
            token->type = VC_TOKEN_COMMENT;
            
            /* Ignore rest of the line */
            while (*PPTR && *PPTR != '\n') PPTR++;
            token->length = (size_t)(PPTR - token->position);
        break;
        case '"': case '\'': {
            char c = *PPTR;
            token->type = VC_TOKEN_STRING;
            PPTR++; (token->position)++;
            while (*PPTR && *PPTR != '\n' && !(*PPTR == c && *(PPTR - 1) != '\\'))PPTR++;
            token->length = (size_t)(PPTR - token->position);
            if (*PPTR != c) {
                token->type = VC_TOKEN_INVALID;
            } else {
                PPTR++;
            }
        } break;
        /* Test for floating point with leading decimal. */
        case '.':
            token->type = VC_TOKEN_FLOAT;
            PPTR++;
        /* Test for integer/floating point type */
        case '-': case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            //int has_decimal = 0;
            if (token->type != VC_TOKEN_FLOAT) token->type = VC_TOKEN_INTEGER;
            
            while (*PPTR && *PPTR != ' ' && *PPTR != '\t' && *PPTR != '\n' && *PPTR != '#' && *PPTR != ';') {
                if (*PPTR == '.') {
                    if (token->type == VC_TOKEN_INTEGER) {
                        token->type = VC_TOKEN_FLOAT;
                    } else {
                        token->type = VC_TOKEN_INVALID;
                    }
                } else if ((*PPTR - '0') < 0 || (*PPTR - '0') > 9) {
                    token->type = VC_TOKEN_INVALID;
                }
                if (*PPTR) PPTR++;
            }
            token->length = (size_t)(PPTR - token->position);
        } break;
        default: {
            int boolval;    /* if we have a boolean value, 1 = true, 0 = false */
            /* We check for an identifier */
            if (is_alpha_char(*PPTR) || is_id_symbol_char(*PPTR)) {
                token->type = VC_TOKEN_IDENTIFIER;
                (PPTR)++;
            } else {
                token->type = VC_TOKEN_INVALID;
            }
            while (*PPTR && *PPTR != ' ' && *PPTR != '\t' && *PPTR != '\n' && *PPTR != '#' && *PPTR != ';' && *PPTR != ']') {
                if (!is_identifier_char(*PPTR)) {
                    token->type = VC_TOKEN_INVALID;
                }
                if (*PPTR) (PPTR)++;
            }
            token->length = (size_t)(PPTR - token->position);
            
            /* Perform additional checks to see if it is a boolean */
            if (is_boolean(token->position, token->length, &boolval)) {
                token->type = VC_TOKEN_BOOLEAN;
                token->length = boolval;
            }
        } break;
    }
    #undef PPTR
    return 1;
}

int strcincmp(char const *a, char const *b, int n)
{
    int i;
    for (i = 0; i < n; i++, a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
    return 0;
}

static unsigned char is_boolean(char *str, size_t length, int *boolval) {
    if (length == 1) {
        if (tolower(*str) == 'f' || tolower(*str) == 'n') {
            *boolval = 0;
            return 1;
        } else if (tolower(*str) == 't' || tolower(*str) == 'y') {
            *boolval = 1;
            return 1;
        }
        return 0;
    } else if (
        (length == 2 && !strcincmp(str, "no", 2))
        || (length == 5 && !strcincmp(str, "false", 5))
    ) {
        *boolval = 0;
        return 1;
    } else if (
        (length == 3 && !strcincmp(str, "yes", 3))
        || (length == 4 && !strcincmp(str, "true", 4))
    ) {
        *boolval = 1;
        return 1;
    }
    return 0;
}

static unsigned char is_identifier_char(char c) {
    return (is_alpha_char(c) || is_digit_char(c) || is_id_symbol_char(c)) ?
            1 : 0;
}

static inline unsigned char is_alpha_char(char c) {
    return ((c - 'A' >= 0 && c - 'A' <= 'Z' - 'A') ||
            (c - 'a' >= 0 && c - 'a' <= 'z' - 'a')) ?
            1 : 0;
}

static inline unsigned char is_digit_char(char c) {
    return (c - '0' >= 0 && c - '0' <= 9) ? 
            1 : 0;
}

static inline unsigned char is_id_symbol_char(char c) {
    return ((c == '-') || (c == '_') || (c == '/') || (c == '\\')) ?
            1 : 0;
}
