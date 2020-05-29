/* gen_vow.i */

%module gen_vow
%{
#include "../src/settings.h"
#include "../src/types/instance.h"
#include "../src/types/state.h"
#include "../src/types/triples.h"
#include "../src/types/bintree.h"
#include "../src/state.h"
#include "../src/storage.h"
#include "../src/gen_vow.h"
%}
 
%include <stdint.i>
%include "../src/settings.h"
%include "../src/types/instance.h"
%include "../src/types/state.h"
%include "../src/types/triples.h"
%include "../src/types/bintree.h"
%include "../src/state.h"
%include "../src/storage.h"
%include "../src/gen_vow.h"

%include <carrays.i>
%array_class(db_ep_coordinates_t, db_ep_coordinates_t_array);
