load "SIDH_StateFunctions.m";

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
    sample : UserProgram,
    /* Isogeny instance */
    e2 : Integers(),
    e3 : Integers(),
    f : Integers(),
    e : Integers(),
    left : SeqEnum, /* For printing params.. */
    right : SeqEnum, /* For printing params.. */
    left_curve : SeqEnum,
    right_curve : SeqEnum,
    strat : SeqEnum,
    lenstrat : Integers(),
    jinv : FldFinElt,
    pbytes : Integers()
>;

// Optain an optimal strategy for traversing the isogeny tree based on relative costs p 
// for l-scalar mult and q for l-isogeny computation. 
function OptStrat(n,p,q)
    S := [[]];
    C := [RealField()!0];
    for i in [2..(n)] do
        newCpqs := [(C[i-b] + C[b] + b*p + (i-b)*q) : b in [1..(i-1)]];  
        newCpq := newCpqs[1];
        b := 1;
        // Choose the cheapest strategy.
        for j in [2..(i-1)] do
            tmpCpq := newCpqs[j];
            if newCpq gt tmpCpq then 
                // strict inequality in the condition prefers larger number of isogenies
                newCpq := tmpCpq;
                b := j;
            end if;
        end for;
  // chosen strategy (m-leave sub-tree on the left, (n-m)-subtree on the right) 
       Append(~S,[b] cat S[i-b] cat S[b]);
       Append(~C, newCpqs[b]);
    end for;

    return S[n];
end function;

function InitParams(e2,e3,f,e,delta,alpha,beta,gamma)
    params := rec< PARAMS | e2 := e2, e3 := e3, f := f, e := e >;

    /* Init vOW parameters */
    params`set_size := 2*3*2^(params`e-1);
    params`log_memory_size := params`e;
    params`theta := alpha*Sqrt(2^(params`log_memory_size)/params`set_size);
    params`max_steps := Ceiling(gamma/params`theta);
    params`max_dist := beta*2^(params`log_memory_size);
    params`max_function_versions := 5;

    /* Init functions */
    params`update := UpdateState;
    params`backtrack := BacktrackState;
    params`distinguished := IsDistinguished;
    params`mem_index := MemIndex;
    params`sample := NewState;

    /* Init isogeny instance */
    assert 2*e le e2;

    p := 2^e2*3^e3*f - 1;
    _<x> := PolynomialRing(GF(p));
    Fp2<i> := ext< GF(p) | x^2 + 1>;

    a0 := Fp2!0;
    E0 := EllipticCurve([Fp2|0,a0,0,1,0]);

    repeat
        P0 := f*Random(E0);
        P1 := 3^e3*P0;
        P2 := 2^(e2-1)*P1;
    until P2 ne E0!0 and P2 ne E0![0,0] and 2*P2 eq E0!0;

    repeat
        Q0 := f*Random(E0);
        Q1 := 3^e3*Q0;
        Q2 := 2^(e2-1)*Q1;
    until Q2 eq E0![0,0];
    P0 := 2^(e2-e)*P1; XP0 := P0[1];
    Q0 := 2^(e2-e)*Q1; XQ0 := Q0[1];
    R0 := 2^(e2-e)*(P1-Q1); XR0 := R0[1];

    assert 2^e*P0 eq E0!0; assert 2^e*Q0 eq E0!0; assert 2^e*R0 eq E0!0;
    assert 2^(e-1)*P0 ne E0!0; assert 2^(e-1)*Q0 ne E0!0; assert 2^(e-1)*R0 ne E0!0;
    assert 2^(e-1)*P0 ne E0![0,0] and 2^(e-1)*Q0 eq E0![0,0];

    // We don't want a subfield curve
    b0 := Random(2); k0 := Random(2^(e-1)-1);
    a0 := SimpleIsogeny(a0,P0,Q0,e,b0,k0);
    E0 := EllipticCurve([Fp2|0,a0,0,1,0]);
    repeat
        P0 := f*Random(E0);
        P1 := 3^e3*P0;
        P2 := 2^(e2-1)*P1;
    until P2 ne E0!0 and P2 ne E0![0,0] and 2*P2 eq E0!0;

    repeat
        Q0 := f*Random(E0);
        Q1 := 3^e3*Q0;
        Q2 := 2^(e2-1)*Q1;
    until Q2 eq E0![0,0];
    P0 := 2^(e2-e)*P1; XP0 := P0[1];
    Q0 := 2^(e2-e)*Q1; XQ0 := Q0[1];
    R0 := 2^(e2-e)*(P1-Q1); XR0 := R0[1];

    assert 2^e*P0 eq E0!0; assert 2^e*Q0 eq E0!0; assert 2^e*R0 eq E0!0;
    assert 2^(e-1)*P0 ne E0!0; assert 2^(e-1)*Q0 ne E0!0; assert 2^(e-1)*R0 ne E0!0;
    assert 2^(e-1)*P0 ne E0![0,0] and 2^(e-1)*Q0 eq E0![0,0];

    // Define curve, midway
    b1_ := Random(2); k1_ := Random(2^(e-1)-1);
    a1_ := SimpleIsogeny(a0,P0,Q0,e,b1_,k1_);
    E1_ := EllipticCurve([Fp2|0,a1_,0,1,0]);
    repeat
        U0_ := f*Random(E1_);
        U1_ := 3^e3*U0_;
        U2_ := 2^(e2-1)*U1_;
    until U2_ ne E1_!0 and U2_ ne E1_![0,0] and 2*U2_ eq E1_!0;

    repeat
        V0_ := f*Random(E1_);
        V1_ := 3^e3*V0_;
        V2_ := 2^(e2-1)*V1_;
    until V2_ eq E1_![0,0];
    U0_ := 2^(e2-e)*U1_; XP1 := U0_[1];
    V0_ := 2^(e2-e)*V1_; XQ1 := V0_[1];
    W0_ := 2^(e2-e)*(U1_-V1_); XR1 := W0_[1];

    assert 2^e*U0_ eq E1_!0; assert 2^e*V0_ eq E1_!0; assert 2^e*W0_ eq E1_!0;
    assert 2^(e-1)*U0_ ne E1_!0; assert 2^(e-1)*V0_ ne E1_!0; assert 2^(e-1)*W0_ ne E1_!0;
    assert 2^(e-1)*U0_ ne E1_![0,0] and 2^(e-1)*V0_ eq E1_![0,0];

    // Set challenge instance
    // Avoid b = 2 because that walks backwards (isogeny formulas defined that way)
    b1 := Random(1); k1 := Random(2^(e-1)-1);
    assert 2^(e-1)*(U0_ + (b1*(2^e div 2 - 1) + k1)*V0_) ne E1_!0;
    a1 := SimpleIsogeny(a1_,U0_,V0_,e,b1,k1);
    E1 := EllipticCurve([Fp2|0,a1,0,1,0]);
    repeat
        U0 := f*Random(E1);
        U1 := 3^e3*U0;
        U2 := 2^(e2-1)*U1;
    until U2 ne E1!0 and U2 ne E1![0,0] and 2*U2 eq E1!0;

    repeat
        V0 := f*Random(E1);
        V1 := 3^e3*V0;
        V2 := 2^(e2-1)*V1;
    until V2 eq E1![0,0];
    U0 := 2^(e2-e)*U1; XP1 := U0[1];
    V0 := 2^(e2-e)*V1; XQ1 := V0[1];
    W0 := 2^(e2-e)*(U1-V1); XR1 := W0[1];

    assert 2^e*U0 eq E1!0; assert 2^e*V0 eq E1!0; assert 2^e*W0 eq E1!0;
    assert 2^(e-1)*U0 ne E1!0; assert 2^(e-1)*V0 ne E1!0; assert 2^(e-1)*W0 ne E1!0;
    assert 2^(e-1)*U0 ne E1![0,0] and 2^(e-1)*V0 eq E1![0,0];

    params`left_curve := [(a0+2)/4, XP0, XQ0, XR0];
    params`right_curve := [(a1+2)/4, XP1, XQ1, XR1];
    params`left := params`left_curve;
    params`right := params`right_curve;
    params`strat := OptStrat(e div 2,1,1);
    params`lenstrat := #params`strat;
    params`jinv := jInvariant(E1_);

    pbits := #IntegerToSequence(p,2);
    params`pbytes := Ceiling(pbits/8);

    return params;
end function;
