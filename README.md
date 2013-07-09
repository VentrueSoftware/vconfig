vconfig
=======

Summary
-------
vconfig is a simple configuration reader/parser library, written in C.  It uses hash tables keyed on the option name for attribute lookup.  It parses and converts types for values in the configuration file.

Config File Format
------------------
    globalOptionA = valueA

    #Comment about section 1
    [Section 1]
        optionA = valueA
        optionB = valueB
        optionC = valueC; optionD = valueD
    [/Section 1]

    #Comment about section 2
    [Section 2]
    optionA = valueA
    optionE = valueE
        [Subsection 1]
        optionX = valueX
        optionY = valueY
        optionZ = valueZ
        [/Subsection 1]
    [/Section 2]
    ...

Vconfig is not whitespace sensitive.  It is line-sensitive; if you
want multiple options on the same line, separate them with semicolons.

Section names and option names can include alphanumeric characters, along with '_', '-', '/', and '\'.
Option values are parsed as the following:

 * String: Anything in quotes
 * Integer: Any numerical string.
 * Float: Any numerical string containing '.'. A decimal point can be the last character (e.g: '42.' = 42.0) or the only character (e.g: '.' = 0.0)
 * Boolean: Any case of true/false.  Evaluates to a char with
                value 1 if true, 0 if false.


Library Usage
-------------
Accessing values is done by passing a string to vc_getopt, and
can access nested values by can be done as follows (assuming vcfg is
the name of your reference to a vc_conf instance):

```C
    vc_opt *val = vc_getopt(vcfg, "section.option");
```
Or:
```C
vconfig *section = vconfig_getval(vcfg, "section");
vc_opt *val = vconfig_getopt(section, "option");
```
(vconfig_getval does not return the containing class, getopt does.)

See vconfig.h for a list of all vconfig_get* functions.

The latter method is better if you'll be referencing the same section
multiple times in an area.

To do
-----
 * Implement automatic hash table resizing.
 * Support multi-line strings.
 * Support arrays.
 * Support directives.
 * Support config merging.
 * Support config exporting.
