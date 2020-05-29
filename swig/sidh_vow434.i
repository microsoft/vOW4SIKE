/* sidh_vow434.i */

%module sidh_vow
%{

#define SWIG_FILE_WITH_INIT
#include <stdint.h>
#define VOW_SIDH
#define P434
#include "../src/types/instance.h"
#include "../src/sidh_vow_base.h"
#include "../src/curve_math.h"
#include "../src/types/state.h"
#include "../src/state.h"
#include "../src/types/bintree.h"

typedef felm_t  f2elm_t[2];

%}

%include "numpy.i"

%init %{
    import_array();
%}

// for passing array in load_f2elm()
%apply ( unsigned long long * IN_ARRAY1, int DIM1 ) {(unsigned long long * in, int len)}

typedef felm_t  f2elm_t[2];

%include <stdint.i>
%define VOW_SIDH
%enddef
%define P434
%enddef
%include "../src/types/instance.h"
%include "../src/sidh_vow_base.h"
%include "../src/curve_math.h"
%include "../src/types/state.h"
%include "../src/state.h"
%include "../src/types/bintree.h"
