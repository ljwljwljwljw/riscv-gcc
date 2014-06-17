/* Test AAPCS64 layout and __builtin_va_start.

   Pass named HFA/HVA argument on stack.  */

/* { dg-do run { target aarch64*-*-* } } */

#ifndef IN_FRAMEWORK
#define AAPCS64_TEST_STDARG
#define TESTFILE "va_arg-13.c"

struct float_float_t
{
  float a;
  float b;
} float_float;

union float_int_t
{
  float b8;
  int b5;
} float_int;

#define HAS_DATA_INIT_FUNC
void
init_data ()
{
  float_float.a = 1.2f;
  float_float.b = 2.2f;

  float_int.b8 = 4983.80f;
}

#include "abitest.h"
#else
  ARG (float, 1.0f, S0, 0)
  ARG (float, 2.0f, S1, 1)
  ARG (float, 3.0f, S2, 2)
  ARG (float, 4.0f, S3, 3)
  ARG (float, 5.0f, S4, 4)
  ARG (float, 6.0f, S5, 5)
  ARG (float, 7.0f, S6, 6)
  ARG (struct float_float_t, float_float, STACK, 7)
  ARG (int,  9, W0, 8)
  ARG (int, 10, W1, 9)
  ARG (int, 11, W2, 10)
  ARG (int, 12, W3, 11)
  ARG (int, 13, W4, 12)
  ARG (int, 14, W5, 13)
  ARG (int, 15, W6, LAST_NAMED_ARG_ID)
  DOTS
  /* Note on the reason of using 'X7' instead of 'W7' here:
     Using 'X7' makes sure the test works in the big-endian mode.
     According to PCS rules B.4 and C.10, the size of float_int is rounded
     to 8 bytes and prepared in the register X7 as if loaded via LDR from
     the memory, with the content of the other 4 bytes unspecified.  The
     test framework will only compare the 4 relavent bytes.  */
  ANON (union float_int_t, float_int, X7, 15)
  LAST_ANON (long long, 12683143434LL, STACK + 8, 16)
#endif
