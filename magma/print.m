procedure PrintFp2(x)
    x1 := ElementToSequence(x)[1];
    x2 := ElementToSequence(x)[2];

    printf "%h\n", Integers()!x1;
    printf "%h\n", Integers()!x2;
end procedure;

procedure PrintFp2_64b(x)
    x1 := ElementToSequence(x)[1];
    x2 := ElementToSequence(x)[2];

    //"Real";
    for i in [0..7] do
        printf "%h, ", ShiftRight(Integers()!x1,64*i) mod 2^64;
    end for;

    "";

    //"Imaginary";
    for i in [0..7] do
        printf "%h, ", ShiftRight(Integers()!x2,64*i) mod 2^64;
    end for;
end procedure;

procedure PrintFp2Mont_64b(x,p,R)
    x1 := Integers()!ElementToSequence(x)[1];
    x2 := Integers()!ElementToSequence(x)[2];

    x1 := ( x1*(2^R) ) mod p;
    x2 := ( x2*(2^R) ) mod p;

    //"Real";
    for i in [0..(R div 64)-2] do
        printf "%h, ", ShiftRight(x1,64*i) mod 2^64;
    end for;
    printf "%h", ShiftRight(x1,64*((R div 64)-1)) mod 2^64;

    "";

    //"Imaginary";
    for i in [0..(R div 64)-2] do
        printf "%h, ", ShiftRight(x2,64*i) mod 2^64;
    end for;
    printf "%h", ShiftRight(x2,64*((R div 64)-1)) mod 2^64;
end procedure;

procedure PrintFp2Mont64b(x,p,R)
    x1 := Integers()!ElementToSequence(x)[1];
    x2 := Integers()!ElementToSequence(x)[2];

    x1 := ( x1*(2^R) ) mod p;
    x2 := ( x2*(2^R) ) mod p;

    //"Real";
    for i in [0..(R div 64)-2] do
        printf "%h, ", ShiftRight(x1,64*i) mod 2^64;
    end for;
    printf "%h, ", ShiftRight(x1,64*((R div 64)-1)) mod 2^64;

    //"Imaginary";
    for i in [0..(R div 64)-2] do
        printf "%h, ", ShiftRight(x2,64*i) mod 2^64;
    end for;
    printf "%h", ShiftRight(x2,64*((R div 64)-1)) mod 2^64;
end procedure;

procedure PrintParams128(e2,e3,f)
    p := 2^e2*3^e3*f-1;
    assert IsPrime(p);
    printf "/* p128 = 2^%o*3^%o*%o - 1 */\n", e2, e3, f;
    printf "const uint64_t p128[NWORDS64_FIELD] = ";
    printf "{ %h, ", p mod 2^64;
    printf "%h };", ShiftRight(p,64) mod 2^64;
    printf "\n";
    printf "const uint64_t p128p1[NWORDS64_FIELD] = ";
    printf "{ %h, ", (p+1) mod 2^64;
    printf "%h };", ShiftRight(p+1,64) mod 2^64;
    printf "\n";
    printf "const uint64_t p128x2[NWORDS64_FIELD] = ";
    printf "{ %h, ", 2*p mod 2^64;
    printf "%h };", ShiftRight(2*p,64) mod 2^64;
    printf "\n";
    printf "const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = ";
    printf "{ %h, ", Integers()!(-1/RingOfIntegers(2^128)!p) mod 2^64;
    printf "%h };", ShiftRight(Integers()!(-1/RingOfIntegers(2^128)!p),64) mod 2^64;
    printf "\n";
    printf "const uint64_t Montgomery_R2[NWORDS64_ORDER] = ";
    printf "{ %h, ", ((2^128)^2 mod p) mod 2^64;
    printf "%h };", ShiftRight((2^128)^2 mod p,64) mod 2^64;
    printf "\n";
    printf "const uint64_t Montgomery_one[NWORDS64_ORDER] = ";
    printf "{ %h, ", ((2^128) mod p) mod 2^64;
    printf "%h };", ShiftRight((2^128) mod p,64) mod 2^64;
end procedure;

procedure PrintVowSidhParams128(list_params)
    printf "#define NUM_INSTS_STATS %o\n", #list_params;
    printf "instance_t insts_stats[NUM_INSTS_STATS] = {\n";
    for ii in [1..#list_params-1] do
        params := list_params[ii];

        e2 := params`e2;
        e3 := params`e3;
        f := params`f;
        
        p := 2^e2*3^e3*f-1;
        assert IsPrime(p);

        logp := #IntegerToSequence(p,2);
        R := ((logp+63) div 64)*64;
        assert R mod 64 eq 0;

        printf "    {\n";
        printf "        .NBITS_K = ";
        printf "%o,\n", params`e-1;
        printf "        .MEMORY_LOG_SIZE = ";
        printf "%o,\n", params`log_memory_size;
        printf "        .ALPHA = ";
        printf "2.25,\n";
        printf "        .BETA = ";
        printf "10.,\n";
        printf "        .GAMMA = ";
        printf "20.,\n";
        printf "        .PRNG_SEED = ";
        printf "1337,\n";
        printf "        .jinv = { "; PrintFp2Mont64b(params`jinv,p,R); printf " },\n";
        printf "        .E = {\n            {\n";
        printf "            .a24 = { "; PrintFp2Mont64b(params`left[1],p,R); printf " },\n";
        printf "            .xp = { "; PrintFp2Mont64b(params`left[2],p,R); printf " },\n";
        printf "            .xq = { "; PrintFp2Mont64b(params`left[3],p,R); printf " },\n";
        printf "            .xpq = { "; PrintFp2Mont64b(params`left[4],p,R); printf " },\n";
        printf "            },\n            {\n";
        printf "            .a24 = { "; PrintFp2Mont64b(params`right[1],p,R); printf " },\n";
        printf "            .xp = { "; PrintFp2Mont64b(params`right[2],p,R); printf " },\n";
        printf "            .xq = { "; PrintFp2Mont64b(params`right[3],p,R); printf " },\n";
        printf "            .xpq = { "; PrintFp2Mont64b(params`right[4],p,R); printf " },\n";
        printf "            }}\n    },\n";
    end for;

    params := list_params[#list_params];

    e2 := params`e2;
    e3 := params`e3;
    f := params`f;
    
    p := 2^e2*3^e3*f-1;
    assert IsPrime(p);

    logp := #IntegerToSequence(p,2);
    R := ((logp+63) div 64)*64;
    assert R mod 64 eq 0;

    printf "    {\n";
    printf "        .NBITS_K = ";
    printf "%o,\n", params`e-1;
    printf "        .MEMORY_LOG_SIZE = ";
    printf "%o,\n", params`log_memory_size;
    printf "        .ALPHA = ";
    printf "2.25,\n";
    printf "        .BETA = ";
    printf "10.,\n";
    printf "        .GAMMA = ";
    printf "20.,\n";
    printf "        .PRNG_SEED = ";
    printf "1337,\n";
    printf "        .jinv = { "; PrintFp2Mont64b(params`jinv,p,R); printf " },\n";
    printf "        .E = {\n            {\n";
    printf "            .a24 = { "; PrintFp2Mont64b(params`left[1],p,R); printf " },\n";
    printf "            .xp = { "; PrintFp2Mont64b(params`left[2],p,R); printf " },\n";
    printf "            .xq = { "; PrintFp2Mont64b(params`left[3],p,R); printf " },\n";
    printf "            .xpq = { "; PrintFp2Mont64b(params`left[4],p,R); printf " },\n";
    printf "            },\n            {\n";
    printf "            .a24 = { "; PrintFp2Mont64b(params`right[1],p,R); printf " },\n";
    printf "            .xp = { "; PrintFp2Mont64b(params`right[2],p,R); printf " },\n";
    printf "            .xq = { "; PrintFp2Mont64b(params`right[3],p,R); printf " },\n";
    printf "            .xpq = { "; PrintFp2Mont64b(params`right[4],p,R); printf " },\n";
    printf "            }}\n    }\n";

    printf "};\n";
end procedure;

procedure PrintVowSikeParams128(list_params)
    printf "#define NUM_INSTS_STATS %o\n", #list_params;
    printf "instance_t insts_stats[NUM_INSTS_STATS] = {\n";
    for ii in [1..#list_params-1] do
        params := list_params[ii];

        e2 := params`e2;
        e3 := params`e3;
        f := params`f;
        
        p := 2^e2*3^e3*f-1;
        assert IsPrime(p);

        logp := #IntegerToSequence(p,2);
        R := ((logp+63) div 64)*64;
        assert R mod 64 eq 0;

        printf "    {\n";
        printf "        .e = ";
        printf "%o,\n", params`e;
        printf "        .MEMORY_LOG_SIZE = ";
        printf "%o,\n", params`log_memory_size;
        printf "        .ALPHA = ";
        printf "2.25,\n";
        printf "        .BETA = ";
        printf "10.,\n";
        printf "        .GAMMA = ";
        printf "20.,\n";
        printf "        .PRNG_SEED = ";
        printf "1337,\n";
        printf "        .delta = ";
        printf "%o,\n", params`delta;
        printf "        .jinv = { "; PrintFp2Mont64b(params`jinv,p,R); printf " },\n";
        printf "        .E = {\n            {\n";
        printf "            .a24 = { "; PrintFp2Mont64b(params`left[1],p,R); printf " },\n";
        printf "            .xp = { "; PrintFp2Mont64b(params`left[2],p,R); printf " },\n";
        printf "            .xq = { "; PrintFp2Mont64b(params`left[3],p,R); printf " },\n";
        printf "            .xpq = { "; PrintFp2Mont64b(params`left[4],p,R); printf " },\n";
        printf "            },\n            {\n";
        printf "            .a24 = { "; PrintFp2Mont64b(params`right[1],p,R); printf " },\n";
        printf "            .xp = { "; PrintFp2Mont64b(params`right[2],p,R); printf " },\n";
        printf "            .xq = { "; PrintFp2Mont64b(params`right[3],p,R); printf " },\n";
        printf "            .xpq = { "; PrintFp2Mont64b(params`right[4],p,R); printf " },\n";
        printf "            }}\n    },\n";
    end for;

    params := list_params[#list_params];

    e2 := params`e2;
    e3 := params`e3;
    f := params`f;
    
    p := 2^e2*3^e3*f-1;
    assert IsPrime(p);

    logp := #IntegerToSequence(p,2);
    R := ((logp+63) div 64)*64;
    assert R mod 64 eq 0;

    printf "    {\n";
    printf "        .e = ";
    printf "%o,\n", params`e;
    printf "        .MEMORY_LOG_SIZE = ";
    printf "%o,\n", params`log_memory_size;
    printf "        .ALPHA = ";
    printf "2.25,\n";
    printf "        .BETA = ";
    printf "10.,\n";
    printf "        .GAMMA = ";
    printf "20.,\n";
    printf "        .PRNG_SEED = ";
    printf "1337,\n";
    printf "        .delta = ";
    printf "%o,\n", params`delta;
    printf "        .jinv = { "; PrintFp2Mont64b(params`jinv,p,R); printf " },\n";
    printf "        .E = {\n            {\n";
    printf "            .a24 = { "; PrintFp2Mont64b(params`left[1],p,R); printf " },\n";
    printf "            .xp = { "; PrintFp2Mont64b(params`left[2],p,R); printf " },\n";
    printf "            .xq = { "; PrintFp2Mont64b(params`left[3],p,R); printf " },\n";
    printf "            .xpq = { "; PrintFp2Mont64b(params`left[4],p,R); printf " },\n";
    printf "            },\n            {\n";
    printf "            .a24 = { "; PrintFp2Mont64b(params`right[1],p,R); printf " },\n";
    printf "            .xp = { "; PrintFp2Mont64b(params`right[2],p,R); printf " },\n";
    printf "            .xq = { "; PrintFp2Mont64b(params`right[3],p,R); printf " },\n";
    printf "            .xpq = { "; PrintFp2Mont64b(params`right[4],p,R); printf " },\n";
    printf "            }}\n    }\n";

    printf "};\n";
end procedure;

procedure PrintVowSikeParams128_py(list_params)
    init_param := list_params[1];
    printf "    \"p_"; 
    printf "%o_", init_param`e2;
    printf "%o\": [\n", init_param`e3;
    for ii in [1..#list_params] do
        params := list_params[ii];

        e2 := params`e2;
        e3 := params`e3;
        f := params`f;
        
        p := 2^e2*3^e3*f-1;
        assert IsPrime(p);

        logp := #IntegerToSequence(p,2);
        R := ((logp+63) div 64)*64;
        assert R mod 64 eq 0;

        printf "        {\n";
        printf "        \"MODULUS\": \"p_";
        printf "%o_", params`e2;
        printf "%o\",\n", params`e3;
        printf "        \"e\": ";
        printf "%o,\n", params`e;
        printf "        \"MEMORY_LOG_SIZE\": ";
        printf "%o,\n", params`log_memory_size;
        printf "        \"ALPHA\": ";
        printf "2.25,\n";
        printf "        \"BETA\": ";
        printf "10.,\n";
        printf "        \"GAMMA\": ";
        printf "20.,\n";
        printf "        \"PRNG_SEED\": ";
        printf "1337,\n";
        printf "        \"delta\": ";
        printf "%o,\n", params`delta;
        printf "        \"jinv\": [ "; PrintFp2Mont64b(params`jinv,p,R); printf " ],\n";
        printf "        \"E\": [{\n";
        printf "            \"a24\": [ "; PrintFp2Mont64b(params`left[1],p,R); printf " ],\n";
        printf "            \"xp\": [ "; PrintFp2Mont64b(params`left[2],p,R); printf " ],\n";
        printf "            \"xq\": [ "; PrintFp2Mont64b(params`left[3],p,R); printf " ],\n";
        printf "            \"xpq\": [ "; PrintFp2Mont64b(params`left[4],p,R); printf " ],\n";
        printf "            },\n            {\n";
        printf "            \"a24\": [ "; PrintFp2Mont64b(params`right[1],p,R); printf " ],\n";
        printf "            \"xp\": [ "; PrintFp2Mont64b(params`right[2],p,R); printf " ],\n";
        printf "            \"xq\": [ "; PrintFp2Mont64b(params`right[3],p,R); printf " ],\n";
        printf "            \"xpq\": [ "; PrintFp2Mont64b(params`right[4],p,R); printf " ],\n";
        printf "            }\n";
        printf "        ]\n";
        printf "        },\n";
    end for;
    printf "    ],"; 

    //params := list_params[#list_params];

    //e2 := params`e2;
    //e3 := params`e3;
    //f := params`f;
    //
    //p := 2^e2*3^e3*f-1;
    //assert IsPrime(p);

    //logp := #IntegerToSequence(p,2);
    //R := ((logp+63) div 64)*64;
    //assert R mod 64 eq 0;

    //printf "    {\n";
    //printf "        .e = ";
    //printf "%o,\n", params`e;
    //printf "        .MEMORY_LOG_SIZE = ";
    //printf "%o,\n", params`log_memory_size;
    //printf "        .ALPHA = ";
    //printf "2.25,\n";
    //printf "        .BETA = ";
    //printf "10.,\n";
    //printf "        .GAMMA = ";
    //printf "20.,\n";
    //printf "        .PRNG_SEED = ";
    //printf "1337,\n";
    //printf "        .delta = ";
    //printf "%o,\n", params`delta;
    //printf "        .jinv = { "; PrintFp2Mont64b(params`jinv,p,R); printf " },\n";
    //printf "        .E = {\n            {\n";
    //printf "            .a24 = { "; PrintFp2Mont64b(params`left[1],p,R); printf " },\n";
    //printf "            .xp = { "; PrintFp2Mont64b(params`left[2],p,R); printf " },\n";
    //printf "            .xq = { "; PrintFp2Mont64b(params`left[3],p,R); printf " },\n";
    //printf "            .xpq = { "; PrintFp2Mont64b(params`left[4],p,R); printf " },\n";
    //printf "            },\n            {\n";
    //printf "            .a24 = { "; PrintFp2Mont64b(params`right[1],p,R); printf " },\n";
    //printf "            .xp = { "; PrintFp2Mont64b(params`right[2],p,R); printf " },\n";
    //printf "            .xq = { "; PrintFp2Mont64b(params`right[3],p,R); printf " },\n";
    //printf "            .xpq = { "; PrintFp2Mont64b(params`right[4],p,R); printf " },\n";
    //printf "            }}\n    }\n";

    //printf "};\n";
end procedure;
