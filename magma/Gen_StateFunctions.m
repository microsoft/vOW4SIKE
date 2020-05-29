/* Functions on state */

STATE := recformat<
    current_state : Integers(), 
    current_steps : Integers(),
    initial_state : Integers(),
    memory : SeqEnum,
    current_dist : Integers(),
    function_version : Integers()
>;

TRIP := recformat<
    current_state : Integers(),
    current_steps : Integers(),
    initial_state : Integers()
>;

// Set it up for a golden collision
function UpdateState(state,params)
    state := state;

    if state`current_state in [0,1] then
        state`current_state := 2;
    else
        s,c := GetSeed();
        SetSeed( (state`current_state + state`function_version) mod 2^32 );
        state`current_state := Random([0..params`set_size-1]);
        SetSeed(s,c); // Return seed to original

        if state`current_state eq 2 then 
            state`current_state +:= 1; 
        end if;
    end if;

    return state;
end function;

function IsDistinguished(state,params)
    return state`current_state le params`theta*params`set_size
        and state`current_state ne 0
        and state`current_state ne 1;
end function;

function MemIndex(state,params)
    return state`current_state mod 2^params`log_memory_size + 1;
end function;

function NewState(state,params)
    state_ := rec< STATE | 
        initial_state := Random([0..params`set_size-1]),
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
        return false, _, _;
    end if; // Robin Hood

    for i in [1..c1`current_steps+1] do
        s0_ := params`update(s0,params);
        s1_ := params`update(s1,params);

        if s0_`current_state eq s1_`current_state then
            if s0_`current_state ne 2 then
                return false, _, _;
            else
                return true, s0, s1;
            end if;
        else
            s0 := s0_;
            s1 := s1_;
        end if;
    end for;
end function;
