####  Makefile for compilation on Linux  ####

OPT=-O3   # Optimization option by default

CC=gcc

# Pick a prime for the P128 implementation. Available options: p_32_20, p_36_22, p_40_25, p_44_27, p_48_30, p_52_33, p_56_35, p_60_38
P128_PRIME=p_32_20

# ld sometimes struggles finding -lgomp. The following variable points it directly there.
LDOMP=-L/usr/lib/x86_64-linux-gnu/ -l:libgomp.so.1

ifeq "$(CC)" "gcc"
    COMPILER=gcc
else ifeq "$(CC)" "clang"
    COMPILER=clang
endif

ARCHITECTURE=_AMD64_
USE_OPT_LEVEL=_FAST_
ifeq "$(ARCH)" "x64"
    ARCHITECTURE=_AMD64_
    USE_OPT_LEVEL=_FAST_
else ifeq "$(ARCH)" "ARM64"
    ARCHITECTURE=_ARM64_
    USE_OPT_LEVEL=_FAST_
    ARM_SETTING=-lrt
endif

ifeq "$(OPT_LEVEL)" "GENERIC"
    USE_OPT_LEVEL=_GENERIC_
endif

ifeq "$(ARCHITECTURE)" "_AMD64_"
	ifeq "$(USE_OPT_LEVEL)" "_FAST_"
		MULX=-D _MULX_
		ifeq "$(USE_MULX)" "FALSE"
			MULX=
		else
			ADX=-D _ADX_
			ifeq "$(USE_ADX)" "FALSE"
				ADX=
			endif
		endif
	endif
endif

ifeq "$(SET)" "EXTENDED"
    ADDITIONAL_SETTINGS=-fwrapv -fomit-frame-pointer -march=native
endif

AR=ar rcs
RANLIB=ranlib

CFLAGS=$(OPT) $(ADDITIONAL_SETTINGS) -D $(ARCHITECTURE) -D __LINUX__ -D $(USE_OPT_LEVEL) -D $(P128_PRIME) $(MULX) $(ADX) -fopenmp -fPIE
LDFLAGS=-lm

ifeq "$(ARCHITECTURE)" "_AMD64_"
ifeq "$(USE_OPT_LEVEL)" "_FAST_"
CFLAGS += -mavx2 -maes -msse2
endif
endif

ifeq "$(USE_OPT_LEVEL)" "_GENERIC_"
    EXTRA_OBJECTS_128=objs128/fp_generic.o
    EXTRA_OBJECTS_434=objs434/fp_generic.o
    EXTRA_OBJECTS_503=objs503/fp_generic.o
    EXTRA_OBJECTS_751=objs751/fp_generic.o
else ifeq "$(USE_OPT_LEVEL)" "_FAST_"
ifeq "$(ARCHITECTURE)" "_AMD64_"
	EXTRA_OBJECTS_128=objs128/fp_x64.o objs128/fp_x64_asm.o
	EXTRA_OBJECTS_434=objs434/fp_x64.o objs434/fp_x64_asm.o
	EXTRA_OBJECTS_503=objs503/fp_x64.o objs503/fp_x64_asm.o
	EXTRA_OBJECTS_751=objs751/fp_x64.o objs751/fp_x64_asm.o
	CFLAGS+= -fPIC
else ifeq "$(ARCHITECTURE)" "_ARM64_"
    EXTRA_OBJECTS_128=objs128/fp_arm64.o objs128/fp_arm64_asm.o
    EXTRA_OBJECTS_434=objs434/fp_arm64.o objs434/fp_arm64_asm.o
    EXTRA_OBJECTS_503=objs503/fp_arm64.o objs503/fp_arm64_asm.o
    EXTRA_OBJECTS_751=objs751/fp_arm64.o objs751/fp_arm64_asm.o
endif
endif
OBJECTS_128=objs128/P128.o $(EXTRA_OBJECTS_128) objs/random.o objs/fips202.o
OBJECTS_434=objs434/P434.o $(EXTRA_OBJECTS_434) objs/random.o objs/fips202.o
OBJECTS_503=objs503/P503.o $(EXTRA_OBJECTS_503) objs/random.o objs/fips202.o
OBJECTS_751=objs751/P751.o $(EXTRA_OBJECTS_751) objs/random.o objs/fips202.o

all: lib128 lib434 lib503 lib751 tests tests_vow tests_vow_sidh tests_vow_sike

objs128/%.o: src/P128/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

objs434/%.o: src/P434/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

objs503/%.o: src/P503/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

objs751/%.o: src/P751/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@


ifeq "$(USE_OPT_LEVEL)" "_GENERIC_"
objs128/fp_generic.o: src/P128/generic/fp_generic.c
	$(CC) -c $(CFLAGS) src/P128/generic/fp_generic.c -o objs128/fp_generic.o

objs434/fp_generic.o: src/P434/generic/fp_generic.c
	$(CC) -c $(CFLAGS) src/P434/generic/fp_generic.c -o objs434/fp_generic.o

objs503/fp_generic.o: src/P503/generic/fp_generic.c
	$(CC) -c $(CFLAGS) src/P503/generic/fp_generic.c -o objs503/fp_generic.o

objs751/fp_generic.o: src/P751/generic/fp_generic.c
	$(CC) -c $(CFLAGS) src/P751/generic/fp_generic.c -o objs751/fp_generic.o
else ifeq "$(USE_OPT_LEVEL)" "_FAST_"
ifeq "$(ARCHITECTURE)" "_AMD64_"
objs128/fp_x64.o: src/P128/AMD64/fp_x64.c
	$(CC) -c $(CFLAGS) src/P128/AMD64/fp_x64.c -o objs128/fp_x64.o

objs128/fp_x64_asm.o: src/P128/AMD64/fp_x64_asm.S
	$(CC) -c $(CFLAGS) src/P128/AMD64/fp_x64_asm.S -o objs128/fp_x64_asm.o

objs434/fp_x64.o: src/P434/AMD64/fp_x64.c
	$(CC) -c $(CFLAGS) src/P434/AMD64/fp_x64.c -o objs434/fp_x64.o

objs434/fp_x64_asm.o: src/P434/AMD64/fp_x64_asm.S
	$(CC) -c $(CFLAGS) src/P434/AMD64/fp_x64_asm.S -o objs434/fp_x64_asm.o

objs503/fp_x64.o: src/P503/AMD64/fp_x64.c
	$(CC) -c $(CFLAGS) src/P503/AMD64/fp_x64.c -o objs503/fp_x64.o

objs503/fp_x64_asm.o: src/P503/AMD64/fp_x64_asm.S
	$(CC) -c $(CFLAGS) src/P503/AMD64/fp_x64_asm.S -o objs503/fp_x64_asm.o

objs751/fp_x64.o: src/P751/AMD64/fp_x64.c
	$(CC) -c $(CFLAGS) src/P751/AMD64/fp_x64.c -o objs751/fp_x64.o

objs751/fp_x64_asm.o: src/P751/AMD64/fp_x64_asm.S
	$(CC) -c $(CFLAGS) src/P751/AMD64/fp_x64_asm.S -o objs751/fp_x64_asm.o
else ifeq "$(ARCHITECTURE)" "_ARM64_"	
    objs128/fp_arm64.o: src/P128/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P128/ARM64/fp_arm64.c -o objs128/fp_arm64.o

    objs128/fp_arm64_asm.o: src/P128/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P128/ARM64/fp_arm64_asm.S -o objs128/fp_arm64_asm.o
			
    objs434/fp_arm64.o: src/P434/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P434/ARM64/fp_arm64.c -o objs434/fp_arm64.o

    objs434/fp_arm64_asm.o: src/P434/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P434/ARM64/fp_arm64_asm.S -o objs434/fp_arm64_asm.o

    objs503/fp_arm64.o: src/P503/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P503/ARM64/fp_arm64.c -o objs503/fp_arm64.o

    objs503/fp_arm64_asm.o: src/P503/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P503/ARM64/fp_arm64_asm.S -o objs503/fp_arm64_asm.o

    objs751/fp_arm64.o: src/P751/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P751/ARM64/fp_arm64.c -o objs751/fp_arm64.o

    objs751/fp_arm64_asm.o: src/P751/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P751/ARM64/fp_arm64_asm.S -o objs751/fp_arm64_asm.o
endif
endif

INDEPENDENT_OBJS=objs/random.o objs/fips202.o objs/storage.o objs/networking.o objs/prng.o objs/gen_vow.o objs/sidh_vow.o objs/sike_vow.o objs/bintree.o objs/memory.o objs/xxhash.o objs/triples.o
objs/random.o: src/random/random.c
objs/fips202.o: src/sha3/fips202.c
objs/storage.o: src/storage.c
objs/networking.o: src/networking.c
objs/prng.o: src/prng.c
objs/gen_vow.o: src/gen_vow.c
objs/sidh_vow.o: src/sidh_vow.c
objs/sike_vow.o: src/sike_vow.c
objs/bintree.o: src/bintree.c
objs/memory.o: src/memory.c
objs/triples.o: src/triples.c
objs/xxhash.o: src/xxhash/xxhash.c

DEPENDENT_OBJS_128=objs/sike_vow128.o objs/sidh_vow128.o objs/memory128.o objs/triples128.o
objs/sike_vow128.o: src/sike_vow.c
objs/sidh_vow128.o: src/sidh_vow.c
objs/memory128.o: src/memory.c
objs/triples128.o: src/triples.c
$(DEPENDENT_OBJS_128):
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -D P128 $< -o $@

DEPENDENT_OBJS_434=objs/sike_vow434.o objs/sidh_vow434.o objs/memory434.o objs/triples434.o
objs/sike_vow434.o: src/sike_vow.c
objs/sidh_vow434.o: src/sidh_vow.c
objs/memory434.o: src/memory.c
objs/triples434.o: src/triples.c
$(DEPENDENT_OBJS_434):
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -D P434 $< -o $@

$(INDEPENDENT_OBJS):
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

lib128: $(OBJECTS_128)
	rm -rf lib128 sike128 sidh128
	mkdir lib128 sike128 sidh128
	$(AR) lib128/libsidh.a $^
	$(RANLIB) lib128/libsidh.a

lib434: $(OBJECTS_434)
	rm -rf lib434 sike434 sidh434
	mkdir lib434 sike434 sidh434
	$(AR) lib434/libsidh.a $^
	$(RANLIB) lib434/libsidh.a

lib503: $(OBJECTS_503)
	rm -rf lib503 sike503 sidh503
	mkdir lib503 sike503 sidh503
	$(AR) lib503/libsidh.a $^
	$(RANLIB) lib503/libsidh.a

lib751: $(OBJECTS_751)
	rm -rf lib751 sike751 sidh751
	mkdir lib751 sike751 sidh751
	$(AR) lib751/libsidh.a $^
	$(RANLIB) lib751/libsidh.a

tests: lib128 lib434 lib503 lib751
	$(CC) $(CFLAGS) -L./lib128 tests/arith_tests-p128.c tests/test_extras.c -lsidh $(LDFLAGS) -o arith_tests-p128 $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib434 tests/arith_tests-p434.c tests/test_extras.c -lsidh $(LDFLAGS) -o arith_tests-p434 $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib503 tests/arith_tests-p503.c tests/test_extras.c -lsidh $(LDFLAGS) -o arith_tests-p503 $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib751 tests/arith_tests-p751.c tests/test_extras.c -lsidh $(LDFLAGS) -o arith_tests-p751 $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib434 tests/test_SIDHp434.c tests/test_extras.c -lsidh $(LDFLAGS) -o sidh434/test_SIDH $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib503 tests/test_SIDHp503.c tests/test_extras.c -lsidh $(LDFLAGS) -o sidh503/test_SIDH $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib751 tests/test_SIDHp751.c tests/test_extras.c -lsidh $(LDFLAGS) -o sidh751/test_SIDH $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib434 tests/test_SIKEp434.c tests/test_extras.c -lsidh $(LDFLAGS) -o sike434/test_SIKE $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib503 tests/test_SIKEp503.c tests/test_extras.c -lsidh $(LDFLAGS) -o sike503/test_SIKE $(ARM_SETTING)
	$(CC) $(CFLAGS) -L./lib751 tests/test_SIKEp751.c tests/test_extras.c -lsidh $(LDFLAGS) -o sike751/test_SIKE $(ARM_SETTING)
	
# AES
AES_OBJS=objs/aes/aes.o objs/aes/aes_c.o
objs/aes/aes.o: src/aes/aes.c
objs/aes/aes_c.o: src/aes/aes_c.c
ifeq "$(ARCHITECTURE)" "_AMD64_"
ifeq "$(USE_OPT_LEVEL)" "_FAST_"
AES_OBJS=objs/aes/aes.o objs/aes/aes_ni.o
objs/aes/aes.o: src/aes/aes.c
objs/aes/aes_ni.o: src/aes/aes_ni.c
endif
endif
$(AES_OBJS):
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

## vOW on a generic random function

vow_gen: objs/gen_vow.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory.o objs/xxhash.o objs/fips202.o objs/triples.o $(AES_OBJS)
	rm -rf vow_gen python/vow_gen
	mkdir vow_gen
	$(AR) vow_gen/libvow_gen.a $^
	$(RANLIB) vow_gen/libvow_gen.a

vow_gen_swig: objs/gen_vow.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory.o objs/xxhash.o objs/fips202.o objs/triples.o $(AES_OBJS)
	make vow_gen
	swig -python -threads -outdir vow_gen swig/gen_vow.i 
	$(CC) $(CFLAGS) -c swig/gen_vow_wrap.c -I/usr/include/python2.7 -o objs/gen_vow_wrap.o
	rm swig/gen_vow_wrap.c 
	ld $(LDOMP) -shared $^ objs/gen_vow_wrap.o -o vow_gen/_gen_vow.so
	touch vow_gen/__init__.py
	rm -f python/vow_gen
	ln -rs vow_gen python/vow_gen

tests_vow: vow_gen
	$(CC) $(CFLAGS) -L./vow_gen tests/test_vOW_gen.c tests/test_extras.c -lvow_gen $(LDFLAGS) -o test_vOW_gen $(ARM_SETTING)

## vOW against an SIDH 2-isogeny tree

vow_sidh: objs/sidh_vow.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory.o objs/xxhash.o objs/fips202.o objs/triples.o $(AES_OBJS)
	rm -rf vow_sidh
	mkdir vow_sidh
	$(AR) vow_sidh/libvow_sidh.a $^
	$(RANLIB) vow_sidh/libvow_sidh.a

vow_sidh128: objs/sidh_vow128.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory128.o objs/xxhash.o objs/fips202.o objs/triples128.o $(AES_OBJS)
	rm -rf vow_sidh
	mkdir vow_sidh
	$(AR) vow_sidh/libvow_sidh.a $^
	$(RANLIB) vow_sidh/libvow_sidh.a

vow_sidh434: objs/sidh_vow434.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory434.o objs/xxhash.o objs/fips202.o objs/triples434.o $(AES_OBJS)
	rm -rf vow_sidh
	mkdir vow_sidh
	$(AR) vow_sidh/libvow_sidh.a $^
	$(RANLIB) vow_sidh/libvow_sidh.a

vow_sidh_swig: $(OBJECTS_128) objs/sidh_vow.o objs/prng.o objs/networking.o objs/storage.o objs/xxhash.o objs/triples.o objs/fips202.o $(AES_OBJS) 
	make lib128
	make vow_sidh
	swig -python -threads -outdir vow_sidh swig/sidh_vow.i 
	
	# fix assignment to arrays, this may very well slow us down
	# could add a new function that just passes pointer over
	sed -i 's/arg2 = \*((f2elm_t \*)(argp2))/fp2copy(*((f2elm_t *)(argp2)),arg2)/g' swig/sidh_vow_wrap.c 
	sed -i 's/(arg1)->a24 = arg2/fp2copy(arg2,(arg1)->a24)/g' swig/sidh_vow_wrap.c
	sed -i 's/(arg1)->xpq = arg2/fp2copy(arg2,(arg1)->xpq)/g' swig/sidh_vow_wrap.c
	sed -i 's/(arg1)->xq = arg2/fp2copy(arg2,(arg1)->xq)/g' swig/sidh_vow_wrap.c
	sed -i 's/(arg1)->xp = arg2/fp2copy(arg2,(arg1)->xp)/g' swig/sidh_vow_wrap.c
	sed -i 's/result =  ((arg1)->a24)/fp2copy((arg1)->a24,result)/g' swig/sidh_vow_wrap.c
	sed -i 's/result =  ((arg1)->xpq)/fp2copy((arg1)->xpq,result)/g' swig/sidh_vow_wrap.c
	sed -i 's/result =  ((arg1)->xq)/fp2copy((arg1)->xq,result)/g' swig/sidh_vow_wrap.c
	sed -i 's/result =  ((arg1)->xp)/fp2copy((arg1)->xp,result)/g' swig/sidh_vow_wrap.c

	# fix warnings wrong casts (not necessary, just less output)
	sed -i 's/arg1 = (unsigned long long (\*)\[2\])(argp1)/fpcopy((digit_t*)(unsigned long long (*)[2])(argp1),(digit_t*)arg1)/g' swig/sidh_vow_wrap.c
	sed -i 's/arg2 = (unsigned long long (\*)\[2\])(argp2)/fpcopy((digit_t*)(unsigned long long (*)[2])(argp2),(digit_t*)arg2)/g' swig/sidh_vow_wrap.c
	sed -i 's/arg3 = (unsigned long long (\*)\[2\])(argp3)/fpcopy((digit_t*)(unsigned long long (*)[2])(argp3),(digit_t*)arg3)/g' swig/sidh_vow_wrap.c
	sed -i 's/arg4 = (unsigned long long (\*)\[2\])(argp4)/fpcopy((digit_t*)(unsigned long long (*)[2])(argp4),(digit_t*)arg4)/g' swig/sidh_vow_wrap.c
	sed -i 's/mp_sub128x2_asm((unsigned long long const \*)arg1,(unsigned long long const \*)arg2,arg3)/mp_sub128x2_asm((digit_t *)arg1,(digit_t *)arg2,arg3)/g' swig/sidh_vow_wrap.c
	sed -i 's/(unsigned long long (\*)\[2\]\[2\])/(digit_t (*)[2][2])/g' swig/sidh_vow_wrap.c
	sed -i 's/(unsigned long long const (\*)\[2\]\[2\])/(const digit_t (*)[2][2])/g' swig/sidh_vow_wrap.c
	sed -i 's/(unsigned long long (\*)\[2\])/(digit_t (*)[2])/g' swig/sidh_vow_wrap.c
	sed -i 's/(unsigned long long const (\*)\[2\])/(const digit_t (*)[2])/g' swig/sidh_vow_wrap.c
	sed -i 's/(unsigned long long (\*))/(digit_t (*))/g' swig/sidh_vow_wrap.c
	sed -i 's/(const unsigned long long (\*))/(const digit_t (*))/g' swig/sidh_vow_wrap.c
	sed -i 's/(unsigned long long const \*)/(const digit_t *)/g' swig/sidh_vow_wrap.c
	# sed -i 's///g' swig/sidh_vow_wrap.c

	$(CC) $(CFLAGS) -c swig/sidh_vow_wrap.c -I/usr/include/python2.7 -o objs/sidh_vow_wrap.o
	rm swig/sidh_vow_wrap.c 

	ld $(LDOMP) -shared $^ objs/sidh_vow_wrap.o -o vow_sidh/_sidh_vow.so 
	touch vow_sidh/__init__.py
	rm -f python/vow_sidh
	ln -rs vow_sidh python/vow_sidh

vow_sidh_swig128: $(OBJECTS_128) objs/sidh_vow128.o objs/prng.o objs/networking.o objs/storage.o objs/xxhash.o objs/triples128.o objs/fips202.o $(AES_OBJS) 
	make lib128
	make vow_sidh128
	swig -python -threads -outdir vow_sidh swig/sidh_vow128.i 

	# Fix array assignments
	sed -i 's/\*(felm_t \*)&arg1->jinv\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->jinv)/g' swig/sidh_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->a24\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->a24)/g' swig/sidh_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xp\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xp)/g' swig/sidh_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xq)/g' swig/sidh_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xpq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xpq)/g' swig/sidh_vow128_wrap.c

	$(CC) $(CFLAGS) -c swig/sidh_vow128_wrap.c -I/usr/include/python2.7 -o objs/sidh_vow128_wrap.o
	rm swig/sidh_vow128_wrap.c 

	ld $(LDOMP) -shared $^ objs/sidh_vow128_wrap.o -o vow_sidh/_sidh_vow.so 
	touch vow_sidh/__init__.py
	rm -f python/vow_sidh
	ln -rs vow_sidh python/vow_sidh

vow_sidh_swig434: $(OBJECTS_434) objs/sidh_vow434.o objs/prng.o objs/networking.o objs/storage.o objs/xxhash.o objs/triples434.o objs/fips202.o $(AES_OBJS) 
	make lib434
	make vow_sidh434
	swig -python -threads -outdir vow_sidh swig/sidh_vow434.i 

	# Fix array assignments
	sed -i 's/\*(felm_t \*)&arg1->jinv\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->jinv)/g' swig/sidh_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->a24\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->a24)/g' swig/sidh_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xp\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xp)/g' swig/sidh_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xq)/g' swig/sidh_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xpq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xpq)/g' swig/sidh_vow434_wrap.c

	$(CC) $(CFLAGS) -c swig/sidh_vow434_wrap.c -I/usr/include/python2.7 -o objs/sidh_vow434_wrap.o
	rm swig/sidh_vow434_wrap.c 

	ld $(LDOMP) -shared $^ objs/sidh_vow434_wrap.o -o vow_sidh/_sidh_vow.so 
	touch vow_sidh/__init__.py
	rm -f python/vow_sidh
	ln -rs vow_sidh python/vow_sidh

tests_vow_sidh128: vow_sidh128 lib128
	$(CC) $(CFLAGS) -D P128 -L./vow_sidh -L./lib128 tests/test_vOW_SIDH.c tests/test_extras.c -lvow_sidh -lsidh $(LDFLAGS) -o test_vOW_SIDH_128 $(ARM_SETTING)

tests_vow_sidh434: vow_sidh434 lib434
	$(CC) $(CFLAGS) -D P434 -L./vow_sidh -L./lib434 tests/test_vOW_SIDH.c tests/test_extras.c -lvow_sidh -lsidh $(LDFLAGS) -o test_vOW_SIDH_434 $(ARM_SETTING)

tests_vow_sidh: tests_vow_sidh128 tests_vow_sidh434

## vOW against a SIKE 2-isogeny tree

vow_sike128: objs/sike_vow128.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory128.o objs/xxhash.o objs/fips202.o objs/triples128.o $(AES_OBJS)
	rm -rf vow_sike
	mkdir vow_sike
	$(AR) vow_sike/libvow_sike.a $^
	$(RANLIB) vow_sike/libvow_sike.a

vow_sike434: objs/sike_vow434.o objs/prng.o objs/networking.o objs/storage.o objs/bintree.o objs/memory434.o objs/xxhash.o objs/fips202.o objs/triples434.o $(AES_OBJS)
	rm -rf vow_sike
	mkdir vow_sike
	$(AR) vow_sike/libvow_sike.a $^
	$(RANLIB) vow_sike/libvow_sike.a

vow_sike: vow_sike128 vow_sike434

vow_sike_swig128: $(OBJECTS_128) objs/sike_vow128.o objs/prng.o objs/networking.o objs/storage.o objs/xxhash.o objs/triples128.o objs/fips202.o $(AES_OBJS)
	make lib128
	make vow_sike128
	swig -python -threads -outdir vow_sike swig/sike_vow128.i 

	# Fix array assignments
	#sed -i 's/arg2 = \*((f2elm_t \*)(argp2))/fp2copy(argp2,arg2)/g' swig/sike_vow128_wrap.c
	#sed -i 's/(arg1)->jinv = arg2/fp2copy(arg2,arg1->jinv)/g' swig/sike_vow128_wrap.c
	#sed -i 's/result =  ((arg1)->jinv)/fp2copy(arg1->jinv,result)/g' swig/sike_vow128_wrap.c
	#sed -i 's/(arg1)->a24 = arg2/fp2copy(arg2,arg1->a24)/g' swig/sike_vow128_wrap.c
	#sed -i 's/(arg1)->xp = arg2/fp2copy(arg2,arg1->xp)/g' swig/sike_vow128_wrap.c
	#sed -i 's/(arg1)->xq = arg2/fp2copy(arg2,arg1->xq)/g' swig/sike_vow128_wrap.c
	#sed -i 's/(arg1)->xpq = arg2/fp2copy(arg2,arg1->xpq)/g' swig/sike_vow128_wrap.c
	#sed -i 's/result =  ((arg1)->a24)/fp2copy(arg1->a24,result)/g' swig/sike_vow128_wrap.c
	#sed -i 's/result =  ((arg1)->xp)/fp2copy(arg1->xp,result)/g' swig/sike_vow128_wrap.c
	#sed -i 's/result =  ((arg1)->xq)/fp2copy(arg1->xq,result)/g' swig/sike_vow128_wrap.c
	#sed -i 's/result =  ((arg1)->xpq)/fp2copy(arg1->xpq,result)/g' swig/sike_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->jinv\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->jinv)/g' swig/sike_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->a24\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->a24)/g' swig/sike_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xp\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xp)/g' swig/sike_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xq)/g' swig/sike_vow128_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xpq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xpq)/g' swig/sike_vow128_wrap.c

	$(CC) $(CFLAGS) -c swig/sike_vow128_wrap.c -I/usr/include/python2.7 -o objs/sike_vow128_wrap.o
	rm swig/sike_vow128_wrap.c 

	ld $(LDOMP) -shared $^ objs/sike_vow128_wrap.o -o vow_sike/_sike_vow.so 
	touch vow_sike/__init__.py
	rm -f python/vow_sike
	ln -rs vow_sike python/vow_sike

vow_sike_swig434: $(OBJECTS_434) objs/sike_vow434.o objs/prng.o objs/networking.o objs/storage.o objs/xxhash.o objs/triples434.o objs/fips202.o $(AES_OBJS) 
	make lib434
	make vow_sike434
	swig -python -threads -outdir vow_sike swig/sike_vow434.i 

	# Fix array assignments
	sed -i 's/\*(felm_t \*)&arg1->jinv\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->jinv)/g' swig/sike_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->a24\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->a24)/g' swig/sike_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xp\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xp)/g' swig/sike_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xq)/g' swig/sike_vow434_wrap.c
	sed -i 's/\*(felm_t \*)&arg1->xpq\[ii\] = \*((felm_t \*)arg2 + ii)/fp2copy(arg2,arg1->xpq)/g' swig/sike_vow434_wrap.c

	$(CC) $(CFLAGS) -c swig/sike_vow434_wrap.c -I/usr/include/python2.7 -o objs/sike_vow434_wrap.o
	rm swig/sike_vow434_wrap.c 

	ld $(LDOMP) -shared $^ objs/sike_vow434_wrap.o -o vow_sike/_sike_vow.so 
	touch vow_sike/__init__.py
	rm -f python/vow_sike
	ln -rs vow_sike python/vow_sike

tests_vow_sike128: vow_sike128 lib128
	$(CC) $(CFLAGS) -D P128 -L./vow_sike -L./lib128 tests/test_vOW_SIKE.c tests/test_extras.c -lvow_sike -lsidh $(LDFLAGS) -o test_vOW_SIKE_128 $(ARM_SETTING)

tests_vow_sike434: vow_sike434 lib434
	$(CC) $(CFLAGS) -D P434 -L./vow_sike -L./lib434 tests/test_vOW_SIKE.c tests/test_extras.c -lvow_sike -lsidh $(LDFLAGS) -o test_vOW_SIKE_434 $(ARM_SETTING)

tests_vow_sike: tests_vow_sike128 tests_vow_sike434

check: tests tests_vow tests_vow_sidh tests_vow_sike

.PHONY: clean

clean:
	rm -rf *.req objs* lib* sidh* sike* arith_tests-* vow_* test_vOW_* python/vow_*
