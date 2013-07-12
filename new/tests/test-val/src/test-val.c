/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 12-Jul-2013
 *    File: vc-test-val.c
 * 
 *    Code to test the value struct.
 */

#include <stdio.h>

#include "vc-common.h"
#include "vc-val.h"

#define DO_TEST(TESTNAME, _TEST_)       \
    printf("Testing %s:\n", TESTNAME);  \
    _TEST_(VCINT)                       \
    _TEST_(VCFLOAT)                     \
    _TEST_(VCCHAR)                      \
    _TEST_(VCSTR)                       \
    
#define TEST_ALLOC(type) {                  \
    VCVAL *val;                             \
    val = new_VCVAL(TYPE_##type, 0);        \
    printf("\tType: %s\t Result: [%s]\n",   \
            #type, val ? "PASS" : "FAIL");  \
    del_VCVAL(val);                         \
}


int main(int argc, char **argv) {
    DO_TEST("VCVAL Allocation", TEST_ALLOC);
    return 0;
}
