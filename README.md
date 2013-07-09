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
 * Boolean: Any case of true/false or yes/no.  Evaluates to an integer with value 1 if true/yes, 0 if false/no.


Library Usage
-------------
### Opening/Closing Config Files
Opening a configuration file is as easy as:

```C
    char *file = "example.cfg";
    vconfig *vcfg = vconfig_open(file);
    
    /* Failure */
    if (!vcfg) {
        printf("Error opening configuration file.\n");
    }
    
    /* Otherwise, we were successful.  Do things here */
    ...
    /* Close the config file - frees all the hash tables and option values. */
    vconfig_close(vcfg);
```
### Accessing values.
Accessing values is done by passing an optpath string to vc_getopt, and
can access nested values as follows:

```C
    /* optpath values are delimited by "." */
    vc_opt *val = vconfig_getopt(vcfg, "section.option");
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

Testing config files
--------------------
If you run "make standalone", you will build a binary in "dist" called
"vconfig", which uses the following command line:

    ./vconfig <filename> [<optpath1> [<optpath2> ...]]

For example, given the configuration file 'test.cfg':

    # This is an example configuration file.
    title = "vconfig example configuration file."
    [section1]
        pi = 3.14159
        
    [/section1]
    # Section names starting with slash (/) require a space between it and
    # the '['.
    [ /home/cgi-bin]
        ExecCgi = true
        ext = ".cgi"
    #There's no ending here, this should cause an error.

vconfig will validate that your configuration file is syntactically correct.

    [VS@SH vconfig]$ dist/vconfig test.cfg
    test.cfg:13: Syntax error: End of file encountered with 1 sections unended.
    Unable to load config file.

By adding the line '[//home/cgi-bin]' to the end of 'test.cfg':

    [VS@SH vconfig]$ dist/vconfig test.cfg
    Config file loaded.

By supplying additional "optpath" strings to vconfig, you can read values
to verify your optpath is correct.  Given the above 'test.cfg' file:

    [VS@SH vconfig]$ dist/vconfig test.cfg title /home/cgi-bin section1.pi
    Config file loaded.
    title = "vconfig example configuration file."
    /home/cgi-bin = <section 0x8b1eab0>
    section1.pi = 3.141590

Using vconfig in your application
---------------------------------
vconfig currently isn't supported as a static/shared object library. It 
comes in the form of a simple .o object file (module).

To use vconfig in your application, you can first run "make" to build the
vconfig.o module (which will be put into "dist").  Then, place the include
headers somewhere and/or add them to the include path of your project. For
instance, if your program is called "test.c" and you place the vconfig
directory into /home/vconfig, you can compile your executable as follows:

    gcc -I/home/vconfig/include test.c /home/vconfig/dist/vconfig.o

If you want to debug vconfig, you can run "make DEBUG=true".

To do
-----
 * Implement automatic hash table resizing.
 * Support multi-line strings.
 * Support arrays.
 * Support directives.
 * Support config merging.
 * Support config exporting.
