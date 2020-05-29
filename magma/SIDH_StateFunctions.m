load "arithmetic.m";
load "step.m";
load "keccak.m";

/* Functions on state */

STATE := recformat<
    current_state : SeqEnum, 
    current_steps : Integers(),
    initial_state : SeqEnum,
    previous_jinvariant : FldFinElt,
    memory : SeqEnum,
    current_dist : Integers(),
    function_version : Integers()
>;

TRIP := recformat<
    current_state : SeqEnum,
    current_steps : Integers(),
    initial_state : SeqEnum
>;

function UpdateState(state,params)
    state := state; // Necessary??

    j := PowerOfPrimeIsogeny(   state`current_state[1],
                                state`current_state[2],
                                state`current_state[3],
                                params`e,
                                params`left_curve,
                                params`right_curve,
                                params`strat,
                                params`lenstrat );

    // Hash j-invariant
    counter := 0;
    out_bytes := Ceiling((params`e+2)/8);
    j_int := [Integers()!c : c in ElementToSequence(j)];
    hash_in_base := IntToByteSeq(j_int[1], params`pbytes) cat IntToByteSeq(j_int[2], params`pbytes);
    hash_in_base := hash_in_base cat IntToByteSeq(state`function_version,8);
    repeat 
        hash_in := hash_in_base cat IntToByteSeq(counter,4);
        h := SHA3_256(hash_in)[1..out_bytes];
        h := SequenceToInteger(h,2^8);
        b := h mod 4; h := h div 4;
        c := h mod 2; h := h div 2;
        k := h mod 2^(params`e-1);
        counter +:= 1;
    until not b eq 3;

    state`current_state[1] := c+1;
    state`current_state[2] := b;
    state`current_state[3] := k;
    state`previous_jinvariant := j;

    return state;
end function;

function HashState(state,params,ds)
    int_in := state`current_state[1] - 1
          + 2*state`current_state[2] 
          + 8*state`current_state[3];
    int_in_len := Ceiling(#IntegerToSequence(int_in,2)/8);
    hash_in := IntToByteSeq(ds,1) cat IntToByteSeq(int_in, int_in_len);

    return SHA3_256(hash_in);
end function;

function IsDistinguished(state,params)
    hash := HashState(state,params,2)[1..4];
    hash_int := SequenceToInteger(hash,2^8);

    return hash_int le 2^32*params`theta;
end function;

function MemIndex(state,params)
    hash := HashState(state,params,3)[1..Ceiling(params`log_memory_size/8)];
    hash_int := SequenceToInteger(hash,2^8) mod 2^params`log_memory_size;

    assert hash_int in [0..2^params`log_memory_size-1];

    return hash_int + 1;
end function;

function NewState(state,params)
    state_ := rec< STATE | 
        initial_state := [Random(1)+1,Random(2),Random(2^(params`e-1)-1)],
        current_steps := 0,
        memory := state`memory,
        current_dist := state`current_dist,
        function_version := state`function_version
    >;
    state_`current_state := state_`initial_state;

    return state_;
end function;

function BacktrackState(c0,c1,fun_ver,params);
    L := c0`current_steps - c1`current_steps;

    if L lt 0 then // Swap values if L < 0
        c0_ := c0;
        c0 := c1;
        c1 := c0_;
        L := -L;
    end if;

    s0 := rec< STATE | current_state := c0`initial_state, function_version := fun_ver >;
    s1 := rec< STATE | current_state := c1`initial_state, function_version := fun_ver >;

    for i in [1..L] do // Catch up the trails
        s0 := params`update(s0,params);
    end for;

    if s0`current_state eq s1`current_state then
        return false, _, _, _;
    end if; // Robin Hood

    for i in [1..c1`current_steps+1] do
        s0_ := params`update(s0,params);
        s1_ := params`update(s1,params);

        if s0_`previous_jinvariant eq s1_`previous_jinvariant then
            if s0`current_state[1] eq s1`current_state[1] then
                return false, _, _, _;
            else
                return true, s0, s1;
            end if;
        else
            s0 := s0_;
            s1 := s1_;
        end if;
    end for;
end function;
