/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 05-Jun-2013
 *    File: vconfig.c
 * 
 * A basic configuration file parser that uses hash tables for attribute
 * lookup. It supports automatic type conversion for values in the
 * configuration file.
 * 
 * TO DO:
 *      - Support multi-line strings.
 *      - Support array types.
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
 * If you want multiple-line strings, do the following:
 * 
 * MultiLine = "This is a "
 *             "multiple-line string."
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
 * Accessing values is done by passing a string to vc_getopt, and
 * can access nested values by can be done as follows (assuming vcfg is
 * the name of your reference to a vc_conf instance):
 * 
 * vc_opt *val = vc_getopt(vcfg, "section.option");
 *                      < OR >
 * vc_conf *vc_sect = vc_getval(vcfg, "section");
 * vc_opt *val = vc_getopt(vc_sect, "option");
 * 
 * (getval does not return the containing class, getopt does).
 * 
 * The latter method is better if you'll be referencing the same section
 * multiple times in an area.
 */

/**********************************************************************/
/**** Includes ********************************************************/
/**********************************************************************/
#include "vconfig.h"
#include "vcdirect.h"

/**********************************************************************/
/**** Macro Definitions ***********************************************/
/**********************************************************************/

/**********************************************************************/
/**** Static Function Prototypes **************************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Function Definitions ********************************************/
/**********************************************************************/

/**********************************************************************/
/******** API Function Definitions ************************************/
/**********************************************************************/
/* Open/Close */
vconfig *vconfig_open(vc_params *params) {
    return vc_parse_file(params);
}
/* Simple open - no directives */
vconfig *vconfig_open_simple(char *file) {
    vc_params p = {file, 0};
    return vc_parse_file(&p);
}

vconfig *vconfig_close(vconfig *vcfg) {
    vc_sect_destroy(vcfg);
    return 0;
}

/* Get option. Returns a generic container struct. */
vc_opt *vconfig_getopt(vconfig *vcfg, char *opt) {
    return vc_getopt(vcfg, opt);
}

/* Get value. Returns the value within the container if known ahead 
 * of time. */
void *vconfig_getval(vconfig *vcfg, char *opt) {
    return vc_getval(vcfg, opt);
}

/* Get a boolean.  If the option path does not exist, or the value is
 * not a boolean, NULL is returned. */
int *vconfig_getbool(vconfig *vcfg, char *optpath) {
	vc_opt *opt = vc_getopt(vcfg, optpath);
	if (opt && opt->type == VC_BOOLEAN) return (int *)opt->value;
	return NULL;
}

/* Get an integer.  If the option path does not exist, or the value is
 * not an integer, NULL is returned. */
int *vconfig_getint(vconfig *vcfg, char *optpath) {
	vc_opt *opt = vc_getopt(vcfg, optpath);
	if (opt && opt->type == VC_INTEGER) return (int *)opt->value;
	return NULL;
}

/* Get a string.  If the option path does not exist, or the value is
 * not an integer, NULL is returned. */
char *vconfig_getstr(vconfig *vcfg, char *optpath) {
	vc_opt *opt = vc_getopt(vcfg, optpath);
	if (opt && opt->type == VC_STRING) return (char *)opt->value;
	return NULL;
}

/* Get a config subsection. If the option path does not exist, or the
 * value is not an integer, NULL is returned. */
vconfig *vconfig_getsect(vconfig *vcfg, char *optpath) {
	vc_opt *opt = vc_getopt(vcfg, optpath);
	if (opt && opt->type == VC_SECTION) return (vconfig *)opt->value;
	return NULL;
}

/**********************************************************************/
/******** Static Function Definitions *********************************/
/**********************************************************************/

#ifdef STANDALONE

#include <stdio.h>


#define VC_DIRECTIVE(name, flags, format)                               \
    {#name, flags, format, VC_DIRECTIVE_##name##_fn__}

#define VC_DEF_DIRECTIVE(name)                                          \
    int VC_DIRECTIVE_##name##_fn__(vc_sect *sect, vc_list *__dir_args)

#define VC_GETARG_INT() *((int *)__dir_args->value); __dir_args = __dir_args->next
#define VC_GETARG_FLOAT() *((double *)__dir_args->value); __dir_args = __dir_args->next
#define VC_GETARG_STR() ((char *)__dir_args->value); __dir_args = __dir_args->next
#define VC_GETARG_BOOL() VC_GETARG_INT()

VC_DEF_DIRECTIVE(addtwo) {
    /* get args */
    int a = VC_GETARG_INT();
    int b = VC_GETARG_INT();
    printf("addtwo: %d + %d = %d\n", a, b, a + b);
    return a + b;    
}

VC_DEF_DIRECTIVE(multwo) {
    int a = VC_GETARG_INT();
    int b = VC_GETARG_INT();
    printf("multwo: %d * %d = %d\n", a, b, a * b);
    return a * b;
}

/* Declare directives */
vc_directive _directives[] = {
    VC_DIRECTIVE(addtwo, 0, "ii"),
    VC_DIRECTIVE(multwo, 0, "ii")
};

int main(int argc, char **argv) {
    vconfig *conf;
    vc_params p;
    int i, a = 3, b = 4;

    vc_list testlist1, testlist2;
    
    if (argc < 2) {
        printf("Usage: %s <filename> [<optpath1> [<optpath2> ...]]\n", argv[0]);
        return 1;
    }
    
    p.file = argv[1];
    p.directives = _directives;
    
    conf = vconfig_open(&p);
    
    if (!conf) printf("Unable to load config file.\n");
    else {
        vc_opt *opt;
        printf("Config file loaded.\n");
        
        testlist1.type = testlist2.type = VC_INTEGER;
        testlist1.value = &a; testlist2.value = &b;
        testlist1.next = &testlist2;
        
        _directives[0].func(0, &testlist1);
        _directives[1].func(0, &testlist1);

        for (i = 2; i < argc; i++) {
            opt = vconfig_getopt(conf, argv[i]);
            if (!opt) {
                printf("%s = <null>\n", argv[i]);
            } else {
                switch (opt->type) {
                    case VC_BOOLEAN:
                        printf("%s = %s\n", argv[i], *((int *)opt->value) ? "TRUE" : "FALSE");
                    break;
                    case VC_INTEGER:
                        printf("%s = %d\n", argv[i], *((int *)opt->value));
                    break;
                    case VC_FLOAT:
                        printf("%s = %lf\n", argv[i], *((double *)opt->value));
                    break;
                    case VC_STRING:
                        printf("%s = \"%s\"\n", argv[i], (char *)opt->value);
                    break;
                    case VC_SECTION:
                        printf("%s = <section %p>\n", argv[i], opt->value);
                    break;
                    default:
                        printf("%s = <unknown type>\n", argv[i]);
                }
            }
        }
        vconfig_close(conf);
    }
    
    return 0;
}

#endif /* #ifdef STANDALONE */
