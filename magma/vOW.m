/* 
 * Test code for vOW algorithm &
 * Printing functions for copying to
 * instances in the C code
 *
 * Uncomment one of "SIKE", "SIDH" or "Generic"
 * to run the vOW algorithm on the respective sets
 */

clear;
load "print.m";

//vow := "SIKE";
//load "SIKE_InitParams.m";

//vow := "SIDH";
//load "SIDH_InitParams.m";

vow := "Generic";
load "Gen_InitParams.m";

function vOW(state,params)
    while state`function_version le params`max_function_versions do
        if state`current_dist ge params`max_dist then
            //print "function_version:", state`function_version;
            state := params`sample(state,params); // Sample new seed
            // Go to new function version
            state`function_version +:= 1;
            state`current_steps := 0;
            state`current_dist := 0;
            state`memory := [];
        elif state`current_steps ge params`max_steps then
            state := params`sample(state,params);
        end if;

        state := params`update(state,params);
        state`current_steps +:= 1;

        if params`distinguished(state,params) then
            state`current_dist +:= 1;
            id := params`mem_index(state,params); // Define a memory position
            trip := rec< TRIP | 
                current_state := state`current_state,
                current_steps := state`current_steps,
                initial_state := state`initial_state >; // No valid universe containing them..
            if not IsDefined(state`memory,id) then // Dist value not encountered before
                state`memory[id] := trip;
                state := params`sample(state,params);
            else
                if      state`memory[id]`current_state eq state`current_state // Not a memory collision
                    and (state`memory[id]`current_steps ne state`current_steps
                    or   state`memory[id]`initial_state ne state`initial_state) then // Not completely equal

                    success,c0,c1 := params`backtrack(state`memory[id],trip,state`function_version,params);
                    if success then "GOLDEN"; state`function_version; state`current_dist; return c0,c1; end if;
                end if;
                
                state`memory[id] := trip;
                state := params`sample(state,params);
            end if;
        end if;
    end while;
end function;

SetSeed(1,3);

alpha := 2.25;
beta := 10;
gamma := 10;

ps := [
    [32,20,23,4,9,0],
    [32,20,23,6,9,0]
    //[32,20,23,12,6],
    //[32,20,23,16,9],
    //[32,20,23,16,9],
    //[32,20,23,16,9],
    //[34,21,109,17,9],
    //[36,22,31,18,10],
    //[38,23,271,19,11],
    //[40,25,71,20,11],
    //[42,26,37,21,12],
    //[44,27,37,22,11],
    //[48,30,13,24,13],
    //[52,33,1,26,15],
    //[56,35,57,28,17],
    //[60,38,57,30,19,0],
    //[216,137,1,18,16,16],
    //[216,137,1,20,16,16],
    //[216,137,1,22,16,16],
    //[216,137,1,24,16,16],
    //[216,137,1,26,16,16],
    //[216,137,1,28,16,16],
    //[216,137,1,30,16,16],
    //[216,137,1,32,16,16],
    //[216,137,1,34,16,16],
    //[216,137,1,36,16,16],
    //[216,137,1,38,16,16],
    //[216,137,1,40,16,16],
    //[216,137,1,42,16,16],
    //[216,137,1,44,16,16],
    //[216,137,1,46,16,16],
    //[216,137,1,48,16,16],
    //[216,137,1,50,16,16],
    //[216,137,1,52,16,16],
    //[216,137,1,54,16,16],
    //[216,137,1,56,16,16],
    //[216,137,1,58,16,16],
    //[216,137,1,60,16,16],
    //[216,137,1,62,16,16],
    //[216,137,1,64,16,16],
    //[216,137,1,66,16,16],
    //[216,137,1,68,16,16],
    //[216,137,1,70,16,16],
    //[216,137,1,72,16,16],
    //[216,137,1,74,16,16],
    //[216,137,1,76,16,16],
    //[216,137,1,78,16,16],
    //[216,137,1,80,16,16]
];

/* Test vOW algorithm */
if vow eq "Generic" then
    print "Generic";
    for i in [1..10] do
        SetSeed(i);

        set_size := 2^8;
        alpha := 1;
        beta := 10;
        gamma := 10;
        
        params := InitParams(set_size,alpha,beta,gamma);
        state := rec< STATE | memory := [], current_dist := 0, function_version := 1 >;
        state := params`sample(state,params);

        time c0,c1 := vOW(state,params);
        assert c0`current_state ne c1`current_state;
        c0_ := params`update(c0,params);
        c1_ := params`update(c1,params);
        assert c0_`current_state eq c1_`current_state;
    end for;
else
    for ps_ in ps do
        e2 := ps_[1]; e3 := ps_[2]; f := ps_[3]; e := ps_[4]; w := ps_[5]; delta := ps_[6];
        params := InitParams(e2,e3,f,e,delta,alpha,beta,gamma);
        params`log_memory_size := w;

        state := rec< STATE | memory := [], current_dist := 0, function_version := 1 >;
        state := params`sample(state,params);

        time c0,c1 := vOW(state,params);
        assert c0`current_state ne c1`current_state;
        c0_ := params`update(c0,params);
        c1_ := params`update(c1,params);
        assert c0_`current_state eq c1_`current_state;
        assert c0_`previous_jinvariant eq params`jinv;
    end for;
end if;

/* 
 * Print parameter sets for C code
 *  - For multiple SIMILAR instances
 */
//if vow ne "Generic" then
//    list_params := [];
//    for ps_ in ps do
//        e2 := ps_[1]; e3 := ps_[2]; f := ps_[3]; e := ps_[4]; w := ps_[5]; delta := ps_[6];
//        params := InitParams(e2,e3,f,e,delta,alpha,beta,gamma);
//        params`log_memory_size := w;
//        Append(~list_params,params);
//        ps_;
//    end for;
//    if vow eq "SIDH" then
//        PrintVowSidhParams128(list_params);
//    elif vow eq "SIKE" then
//        PrintVowSikeParams128_py(list_params);
//    end if;
//end if;

/* 
 * Print parameter sets for C code
 *  - For multiple SINGLE instances
 */
//if vow ne "Generic" then
//    for ps_ in ps do
//        e2 := ps_[1]; e3 := ps_[2]; f := ps_[3]; e := ps_[4]; w := ps_[5]; delta := ps_[6];
//        params := InitParams(e2,e3,f,e,delta,alpha,beta,gamma);
//        params`log_memory_size := w;
//        if vow eq "SIDH" then
//            PrintVowSidhParams128([params]);
//        elif vow eq "SIKE" then
//            //PrintVowSikeParams128([params]);
//            PrintVowSikeParams128_py([params]);
//        end if;
//    end for;
//end if;
