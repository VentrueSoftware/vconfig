/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 05-Jun-2013
 *    File: vconfig.h
 * 
 * A basic configuration file parser that uses hash tables for attribute
 * lookup. It supports automatic type conversion for values in the
 * configuration file.
 * 
 * The format of configuration files is as follows:
 * ------------------------------------------------
 * globalOptionA = valueA
 * 
 * #Comment about section 1
 *  [Section 1]
 *  optionA = valueA
 *  optionB = valueB
 *  optionC = valueC; optionD = valueD
 *  [/Section 1]
 * 
 *  #Comment about section 2
 *  [Section 2]
 *  optionA = valueA
 *  optionE = valueE
 *      [Subsection 1]
 *      optionX = valueX
 *      optionY = valueY
 *      optionZ = valueZ
 *      [/Subsection 1]
 *  [/Section 2]
 * ...
 * ------------------------------------------------
 * 
 * Vconfig is not whitespace sensitive.  It is line-sensitive; if you
 * want multiple options on the same line, separate them with semicolons.
 * 
 * Section can contain anything except '.', '[', and ']', and can be nested.
 * Option names should not contain ' ', '.', '[', and ']'.  Option values
 * are parsed as the following:
 * 
 *      - String: Anything in quotes
 *      - Integer: Any numerical string.
 *      - Boolean: Any case of true/false.  Evaluates to a char with
 *                 value 1 if true, 0 if false.
 * 
 * Accessing values is done by passing a string to vconfig_getopt, and
 * can access nested values by can be done as follows (assuming vcfg is
 * the name of your reference to a vc_conf instance):
 * 
 * vc_opt *val = vconfig_getopt(vcfg, "section.option");
 *                      < OR >
 * vc_conf *vc_sect = vc_getval(vcfg, "section");
 * vc_opt *val = vconfig_getopt(vc_sect, "option");
 * 
 * (getval does not return the containing class, getopt does).
 * 
 * The latter method is better if you'll be referencing the same section
 * multiple times in an area.
 */

#ifndef __VCONFIG_H
#define __VCONFIG_H

/**********************************************************************/
/**** Begin Includes **************************************************/
/**********************************************************************/
#include "vctype.h"     /* For types */
#include "vcparse.h"    /* For parse methods */

/**********************************************************************/
/**** Begin Type Definitions ******************************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Begin Definitions/Static Declarations ***************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Begin Function Prototypes ***************************************/
/**********************************************************************/

/* Open/Close */
vconfig *vconfig_open(vc_params *params);
vconfig *vconfig_open_simple(char *file);
vconfig *vconfig_close(vconfig *vcfg);

/* Get option. Returns a generic container struct. */
vc_opt *vconfig_getopt(vconfig *vcfg, char *opt);

/* Get value. Returns the value within the container if known ahead 
 * of time. */
void *vconfig_getval(vconfig *vcfg, char *opt);

/* Get VConfig option value.  You must know the type ahead of time for
 * this one. */
void *vconfig_getval(vconfig *vcfg, char *optpath);

/* Get a boolean.  If the option path does not exist, or the value is
 * not a boolean, NULL is returned. */
int *vconfig_getbool(vconfig *vcfg, char *optpath);

/* Get an integer.  If the option path does not exist, or the value is
 * not an integer, NULL is returned. */
int *vconfig_getint(vconfig *vcfg, char *optpath);

/* Get a string.  If the option path does not exist, or the value is
 * not an integer, NULL is returned. */
char *vconfig_getstr(vconfig *vcfg, char *optpath);

/* Get a config subsection. If the option path does not exist, or the
 * value is not an integer, NULL is returned. */
vconfig *vconfig_getsect(vconfig *vcfg, char *optpath);
#endif /* #ifndef __VCONFIG_H */
