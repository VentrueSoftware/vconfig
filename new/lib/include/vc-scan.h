/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 12-Jul-2013
 *    File: vc-scan.h
 * 
 *    Scanner for VConfig files.
 */

struct scanner_state;
typedef struct scanner_state VCSCANNER;

struct scanner_state {
    /* Scanner positions; begin and end allows for stream-based scanning. */
    VCBYTE *begin;
    VCBYTE *end;
    
    /* Scanner positional data */
    VCCNT line;         /* Line number */
    VCBYTE *line_beg;    /* Start of line */
    
    /* Error data */
    VCCNT errors;
};
