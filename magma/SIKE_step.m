function PowerOfPrimeIsogeny(c,b,k,LEN,DELTA,C0,C1,STRAT,LENSTRAT)

    if c eq 1 then 
        a24,xp,xq,xpq := Explode(C0[k mod 2^(DELTA+1)+1]); // mod 2^(delta+1)
        k_ := ShiftRight(k,DELTA+1);
    elif c eq 2 then // Ignore LSB, which isn't used
        a24,xp,xq,xpq := Explode(C1[k mod 2^DELTA + 1]); // mod 2^delta
        k_ := ShiftRight(k,DELTA);
    end if;


    if DELTA + 2 ne LEN then
        X,Z := LADDER_3_pt(k_,xp,xq,xpq,a24);

        //LENSTRAT;
        A24 := a24; 
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
        A24,C24 := GetFourIsogenyWithKernelXneZ(X,Z); 

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
    else
        j := a24;
    end if;

    return j;
end function;
