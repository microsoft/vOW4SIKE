load "SIKE_StateFunctions.m";
load "FindBasis-E1.m";

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
    delta : Integers(), /* Depth of pre-computation */
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

    if n eq 0 then
        return [];
    else
        return S[n];
    end if;
end function;

function IsogenyWithPoints(A24,X,Z,XP,ZP,XQ,ZQ,XPQ,ZPQ,STRAT,LENSTRAT)
    C24 := 1;
	pts := [];
	index := 0;
    ii := 1;

    // Alice's main loop
    for row := 1 to LENSTRAT do
        // multiply (X:Z) until it has order 4, and store intermediate points
        while index lt (LENSTRAT + 1 - row) do
            Append(~pts, [X,Z,index]);
            m := STRAT[ii]; ii +:= 1;
            X,Z := xDBLe(X,Z,A24,C24,2*m);
            index +:= m;
        end while;

        // compute the 4-isogeny based on kernel (X:Z)
        A24,C24,K1,K2,K3 := GetFourIsogenyWithKernelXneZ(X,Z); 
        XP,ZP := EvalFourIsogenyWithKernelXneZ(XP,ZP,K1,K2,K3);
        XQ,ZQ := EvalFourIsogenyWithKernelXneZ(XQ,ZQ,K1,K2,K3);
        XPQ,ZPQ := EvalFourIsogenyWithKernelXneZ(XPQ,ZPQ,K1,K2,K3);

        // evaluate the 4-isogeny at every point in pts
        for i:=1 to #pts do 
            pts[i][1],pts[i][2] := EvalFourIsogenyWithKernelXneZ(pts[i][1],pts[i][2],K1,K2,K3); 
        end for;

        // R becomes the last point in pts and then pts is pruned
        X := pts[#pts][1]; 
        Z := pts[#pts][2]; 
        index := Integers()!pts[#pts][3];	

        Prune(~pts);
    end for;

    // compute the last 4-isogeny
	A24,C24,K1,K2,K3 := GetFourIsogenyWithKernelXneZ(X,Z); 
    XP,ZP := EvalFourIsogenyWithKernelXneZ(XP,ZP,K1,K2,K3);
    XQ,ZQ := EvalFourIsogenyWithKernelXneZ(XQ,ZQ,K1,K2,K3);
    XPQ,ZPQ := EvalFourIsogenyWithKernelXneZ(XPQ,ZPQ,K1,K2,K3);

    return A24,C24,XP,ZP,XQ,ZQ,XPQ,ZPQ;
end function;

function InitParams(e2,e3,f,e,delta,alpha,beta,gamma)
    params := rec< PARAMS | e2 := e2, e3 := e3, f := f, e := e >;

    /* Init isogeny instance */
    assert 2*e le e2;

    p := 2^e2*3^e3*f - 1;
    _<x> := PolynomialRing(GF(p));
    Fp2<i> := ext< GF(p) | x^2 + 1>;

    /* For printing... */
    logp := #IntegerToSequence(p,2);
    R := ((logp+63) div 64)*64;
    assert R mod 64 eq 0;

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

    //"\n";
    //PrintFp2Mont_64b((a0+2)/4);
    //"\n";
    //PrintFp2Mont_64b(XP0);
    //"\n";
    //PrintFp2Mont_64b(XQ0);
    //"\n";
    //PrintFp2Mont_64b(XR0);
    //"\n";

    // Define curve, midway with kernel neq (0,0)
    b1_ := Random(1); k1_ := Random(2^(e-1)-1);
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
    assert 2^(e-1)*(U0_ + (b1*(2^(e div 2) - 1) + k1)*V0_) ne E1_!0;
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
        V2 := 2^(e2-2)*V1;
    until V2[1] eq 1; /* NOTICE THAT V2 LIES ABOVE 1, NOT JUST 0 */
    U0 := 2^(e2-e)*U1; XP1 := U0[1];
    V0 := 2^(e2-e)*V1; XQ1 := V0[1];
    W0 := 2^(e2-e)*(U1-V1); XR1 := W0[1];

    assert 2^e*U0 eq E1!0; assert 2^e*V0 eq E1!0; assert 2^e*W0 eq E1!0;
    assert 2^(e-1)*U0 ne E1!0; assert 2^(e-1)*V0 ne E1!0; assert 2^(e-1)*W0 ne E1!0;
    assert 2^(e-1)*U0 ne E1![0,0] and 2^(e-1)*V0 eq E1![0,0];

    params`right := [(a1+2)/4,XP1,XQ1,XR1];

    //"\nBasis points input:";
    ////"\n";
    //PrintFp2Mont_64b((a1+2)/4,p,R);
    //"\n";
    //PrintFp2Mont_64b(XP1,p,R);
    //"\n";
    //PrintFp2Mont_64b(XQ1,p,R);
    //"\n";
    //PrintFp2Mont_64b(XR1,p,R);
    //"\n";

    /* Set things up to make the step 1 smaller from both sides */

    first_step := [];
    params`left_curve := [];

    /* For assertions */
    a0_ := Fp2!6;
    E0_ := EllipticCurve([Fp2|0,a0_,0,1,0]);
    /* End assertions */

    //a24_ := (Fp2!6+2)/4;
    //XP0_ := Fp2!1508098487228380685564166832024142451438949267535434447752985105714525167520892792996448173304900148372330756079044863633448731672048087652233259585515;
    //XQ0_ := Fp2!1885478805744890192727197471233965824086732237321044250572761021318566209272333271134611200965625128001864239232948462289136811260308694013854926963754;
    //XR0_ := 1096619296794899152943733428410501304870342415980462750365638092440747073863331366027265826179714926562738943022475985750637334176666665774506458839152*i + 10024853074906578028249554633388653302797169354991138133846595212863867295499348667855370064368543143960340237903690265687977846939857560442590956138677;

    ///* Assertions: check points */
    //YP0_ := Fp2!9757561883926982256396661262683741124780920847182162606359070773733994411609154108400443144199873771012634935322508500195557180815208550119615451758643*i;
    //P0_ := E0_![XP0_,YP0_];

    //YQ0_ := Fp2!12106453190517708755129823173939821817437908801909263822038532678943238128129722298945362188961728246807256213182569530246202569320768966763127731372521;
    //Q0_ := E0_![XQ0_,YQ0_];
    //R0_ := P0_-Q0_;

    left := GetBasis(e2,e3,f,e-1,Fp2,params);

    a24_ := left[1];
    XP0_ := left[2]; ZP0_ := 1;
    XQ0_ := left[3]; ZQ0_ := 1;
    XR0_ := left[4]; ZR0_ := 1;

    YP0_ := Sqrt(XP0_^3 + a0_*XP0_^2 + XP0_);
    P0_ := E0_![XP0_,YP0_];
    YQ0_ := Sqrt(XQ0_^3 + a0_*XQ0_^2 + XQ0_);
    Q0_ := E0_![XQ0_,YQ0_];
    YR0_ := Sqrt(XR0_^3 + a0_*XR0_^2 + XR0_);
    R0_ := E0_![XR0_,YR0_];

    if R0_[1] ne (P0_-Q0_)[1] then Q0_ := -Q0_; end if;
    if R0_ ne P0_-Q0_ then R0_ := -R0_; end if;
    assert R0_ eq P0_-Q0_;

    assert XR0_ eq R0_[1];
    assert P0_[1]^p eq P0_[1] and P0_[2]^p eq -P0_[2];
    assert Q0_[1]^p eq Q0_[1] and Q0_[2]^p eq Q0_[2];
    assert 2^(e-2)*P0_ ne E0_![0,0] and 2^(e-2)*P0_ ne E0_!0 and 2^(e-2)*Q0_ eq E0_![0,0];
    assert 2^(e-1)*P0_ eq E0!0 and 2^(e-1)*Q0_ eq E0_!0;
    /* End assertions */

    /* Get basis of right order 2^(e-1) */

    params`left := [a24_,XP0_,XQ0_,XR0_];

    //"Left basis";
    //"\n";
    //PrintFp2Mont_64b(a24_,p,R);
    //"\n";
    //PrintFp2Mont_64b(XP0_/ZP0_,p,R);
    //"\n";
    //PrintFp2Mont_64b(XQ0_/ZQ0_,p,R);
    //"\n";
    //PrintFp2Mont_64b(XR0_/ZR0_,p,R);
    //"\n";

    XP := XP0_/ZP0_; ZP := 1;
    XQ,ZQ,XPQ,ZPQ := xDBLADD(XQ0_/ZQ0_,1,XR0_/ZR0_,1,XP0_/ZP0_,a24_);
    XR,ZR := xDBLe(XP,ZP,a24_,1,e-2);

    /* Assertions: check points */
    assert XP/ZP eq P0_[1];
    assert XQ/ZQ eq (2*Q0_)[1];
    assert XPQ/ZPQ eq (P0_-2*Q0_)[1];
    assert XR/ZR eq (2^(e-2)*P0_)[1];
    /* End assertions */

    A24,C24 := GetTwoIsogenyWithKernelXneZ(XR,ZR);
    XP,ZP := EvalTwoIsogenyWithKernelXneZ(XR,ZR,XP,ZP);
    XQ,ZQ := EvalTwoIsogenyWithKernelXneZ(XR,ZR,XQ,ZQ);
    XPQ,ZPQ := EvalTwoIsogenyWithKernelXneZ(XR,ZR,XPQ,ZPQ);

    a24 := A24/C24;
    xp := XP/ZP;
    xq := XQ/ZQ;
    xpq := XPQ/ZPQ;

    first_step[1] := [a24,xp,xq,xpq];

    XQ,ZQ,XP,ZP := xDBLADD(XQ0_,ZQ0_,XP0_,ZP0_,XR0_/ZR0_,a24_);
    XPQ := XR0_; ZPQ := ZR0_;
    XR,ZR := xDBLe(XP,ZP,a24_,1,e-2);

    /* Assertions: check points */
    assert XP/ZP eq (P0_+Q0_)[1];
    assert XQ/ZQ eq (2*Q0_)[1];
    assert XPQ/ZPQ eq (P0_-Q0_)[1];
    assert XR/ZR eq (2^(e-2)*(P0_+Q0_))[1];
    /* End assertions */

    A24,C24 := GetTwoIsogenyWithKernelXneZ(XR,ZR);
    XP,ZP := EvalTwoIsogenyWithKernelXneZ(XR,ZR,XP,ZP);
    XQ,ZQ := EvalTwoIsogenyWithKernelXneZ(XR,ZR,XQ,ZQ);
    XPQ,ZPQ := EvalTwoIsogenyWithKernelXneZ(XR,ZR,XPQ,ZPQ);

    a24 := A24/C24;
    xp := XP/ZP;
    xq := XQ/ZQ;
    xpq := XPQ/ZPQ;

    first_step[2] := [a24,xp,xq,xpq];

    if delta eq 0 then
        params`left_curve[1] := first_step[1];
        params`left_curve[2] := first_step[2];
    elif delta gt 0 then
        for k in [0..2^(delta+1)-1] do
        //for k in [56454..56454] do
            a24 := first_step[BitwiseAnd(k,1)+1][1];
            XP0 := first_step[BitwiseAnd(k,1)+1][2];
            XQ0 := first_step[BitwiseAnd(k,1)+1][3];
            XR0 := first_step[BitwiseAnd(k,1)+1][4];
            _,_,XPpQ,ZPpQ := xDBLADD(XP0,1,XQ0,1,XR0,a24);

            k_ := ShiftRight(k,1);
            XP,ZP := LADDER_3_pt(k_,XP0,XQ0,XR0,a24);
            XQ,ZQ := xDBLe(XQ0,Parent(XQ0)!1,a24,Parent(a24)!1,delta);
            XPQ,ZPQ := LADDER_3_pt(2^delta-k_,XP0,XQ0,XPpQ/ZPpQ,a24);
            XR,ZR := xDBLe(XP,ZP,a24,1,e-2-delta);

            strat := OptStrat(delta div 2,1,1);
            lenstrat := #strat;
            A24,C24,XP,ZP,XQ,ZQ,XPQ,ZPQ :=
                IsogenyWithPoints(a24,XR,ZR,XP,ZP,XQ,ZQ,XPQ,ZPQ,strat,lenstrat);

            if delta + 2 ne e then
                a24 := A24/C24;
                xp := XP/ZP;
                xq := XQ/ZQ;
                xpq := XPQ/ZPQ;

                params`left_curve[k+1] := [a24,xp,xq,xpq];
            else

                A24 := A24 + A24;
                A24 := A24 - C24;
                A24 := A24 + A24;

                j := j_inv(A24,C24);

                j0 := ElementToSequence(j)[1];
                j1 := ElementToSequence(j)[2];
                if IsOdd(Integers()!j1) then
                    j1 := -j1;
                end if;
                j := Parent(j)![j0,j1];

            //if k eq 56454 then
            //    "Here..";
            //    k;
            //    PrintFp2Mont_64b(j,p,R);
            //    "\n";
            //end if;

                params`left_curve[k+1] := [j,Fp2!0,Fp2!0,Fp2!0];
            end if;
        end for;
    end if;

    /* Right side */

    // 1 step from the right with kernel x = 1, and push through basis
    A224,C224 := GetFourIsogenyWithKernelXeqZ((a1+2)/4);

    _,_,XPpQ,ZPpQ := xDBLADD(XP1,1,XQ1,1,XR1,(a1+2)/4);
    XR1,ZR1 := LADDER_3_pt(4,XQ1,XP1,XPpQ/ZPpQ,(a1+2)/4);
    XP1,ZP1 := xDBLe(XP1,1,(a1+2)/4,1,2);

    assert XP1/ZP1 eq (4*U0)[1];
    assert XR1/ZR1 eq (V0-4*U0)[1];

    /* The image of *P* will lie above (0,0), so put that into XQ2 */
    XQ2,ZQ2 := EvalFourIsogenyWithKernelXeqZ((a1+2)/4,XP1,ZP1);
    XP2,ZP2 := EvalFourIsogenyWithKernelXeqZ((a1+2)/4,XQ1,1);
    XR2,ZR2 := EvalFourIsogenyWithKernelXeqZ((a1+2)/4,XR1,ZR1);

    a224 := A224/C224;
    XP2 := XP2/ZP2;
    XQ2 := XQ2/ZQ2;
    XR2 := XR2/ZR2;

    //"\nAfter first step:";
    //"\n";
    //PrintFp2Mont_64b(a224,p,R);
    //"\n";
    //PrintFp2Mont_64b(XP2,p,R);
    //"\n";
    //PrintFp2Mont_64b(XQ2,p,R);
    //"\n";
    //PrintFp2Mont_64b(XR2,p,R);
    //"\n";

    params`right_curve := [];
    if delta eq 0 then
        params`right_curve[1] := [a224,XP2,XQ2,XR2];
    elif delta gt 0 then
        _,_,XPpQ,ZPpQ := xDBLADD(XP2,1,XQ2,1,XR2,a224);
        for k in [0..2^delta-1] do
        //for k in [0xd42..0xd42] do

            A24 := a224; C24 := 1;
            XP,ZP := LADDER_3_pt(k,XP2,XQ2,XR2,a224);
            XQ,ZQ := xDBLe(XQ2,Parent(XQ2)!1,a224,Parent(a224)!1,delta);
            XPQ,ZPQ := LADDER_3_pt(2^delta-k,XP2,XQ2,XPpQ/ZPpQ,a224);
            XR,ZR := xDBLe(XP,ZP,a224,1,e-2-delta);
            XP0,ZP0 := xDBLe(XP2,ZP2,a224,1,e-2-delta);
            XQ0,ZQ0 := xDBLe(XQ2,ZQ2,a224,1,e-2-delta);
            XPQ0,ZPQ0 := xDBLe(XR2,ZR2,a224,1,e-2-delta);

            //"P0:";
    //PrintFp2Mont_64b(XP0,p,R);
    //"\n";
    //PrintFp2Mont_64b(ZP0,p,R);
    //"\n";
    //        "Q0:";
    //PrintFp2Mont_64b(XQ0,p,R);
    //"\n";
    //PrintFp2Mont_64b(ZQ0,p,R);
    //"\n";
    //        "PQ0:";
    //PrintFp2Mont_64b(XPQ0,p,R);
    //"\n";
    //PrintFp2Mont_64b(ZPQ0,p,R);
    //"\n";


            strat := OptStrat(delta div 2,1,1);
            lenstrat := #strat;
            A24,C24,XP,ZP,XQ,ZQ,XPQ,ZPQ :=
                IsogenyWithPoints(a224,XR,ZR,XP,ZP,XQ,ZQ,XPQ,ZPQ,strat,lenstrat);

            //"k = ", k;
            a24 := A24/C24;

            if delta + 2 ne e then
                xp := XP/ZP;
                xq := XQ/ZQ;
                xpq := XPQ/ZPQ;
                

    //PrintFp2Mont_64b(a24,p,R);
    //"\n";
    //PrintFp2Mont_64b(xp,p,R);
    //"\n";
    //PrintFp2Mont_64b(xq,p,R);
    //"\n";
    //PrintFp2Mont_64b(xpq,p,R);
    //"\n";


                params`right_curve[k+1] := [a24,xp,xq,xpq];
            else
                A24 := a24; C24 := 1;

                A24 := A24 + A24;
                A24 := A24 - C24;
                A24 := A24 + A24;

                j := j_inv(A24,C24);

                j0 := ElementToSequence(j)[1];
                j1 := ElementToSequence(j)[2];
                if IsOdd(Integers()!j1) then
                    j1 := -j1;
                end if;
                j := Parent(j)![j0,j1];

            //if k eq 0xd42 then
            //    "Here..";
            //    k;
            //    PrintFp2Mont_64b(j,p,R);
            //    "\n";
            //end if;

                params`right_curve[k+1] := [j,Fp2!0,Fp2!0,Fp2!0];
            end if;
        end for;
    end if;

    /* Init vOW parameters */
    params`set_size := 2*2^(params`e-2);
    params`log_memory_size := params`e-2;
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

    params`delta := delta;
    params`strat := OptStrat((e-2-delta) div 2,1,1);
    params`lenstrat := #params`strat;

    j := jInvariant(E1_);
    j0 := ElementToSequence(j)[1];
    j1 := ElementToSequence(j)[2];
    J1 := ( Integers()!j1*(2^R) ) mod p; /* Check sign on Montgomery representation */
    if IsOdd(J1) then
        j1 := -j1;
    end if;
    j := Parent(j)![j0,j1];

    params`jinv := j;

    //"\n";
    //PrintFp2Mont_64b(j,p,R);
    //"\n";

    pbits := #IntegerToSequence(p,2);
    params`pbytes := Ceiling(pbits/8);

    return params;
end function;
