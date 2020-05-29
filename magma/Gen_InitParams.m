load "Gen_StateFunctions.m";

/* Initialization */

PARAMS := recformat<
    /* Generic vOW params */
    set_size : Integers(),
    log_memory_size : Integers(),
    theta : RealField(20),
    max_steps : Integers(),
    max_dist : Integers(),
    max_function_versions : Integers(),
    /* Functions that act on state */
    update : UserProgram,
    backtrack : UserProgram,
    distinguished : UserProgram,
    mem_index : UserProgram,
    sample : UserProgram
>;

function InitParams(set_size,alpha,beta,gamma)
    params := rec< PARAMS | >;

    /* Init vOW parameters */
    params`set_size := set_size;
    params`log_memory_size := Ceiling(Log(2,set_size)/2);
    params`theta := alpha*Sqrt(2^(params`log_memory_size)/params`set_size);
    params`max_steps := Ceiling(gamma/params`theta);
    params`max_dist := beta*2^(params`log_memory_size);
    params`max_function_versions := 1000;

    /* Init functions */
    params`update := UpdateState;
    params`backtrack := BacktrackState;
    params`distinguished := IsDistinguished;
    params`mem_index := MemIndex;
    params`sample := NewState;

    return params;
end function;
