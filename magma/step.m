function PowerOfPrimeIsogeny(c,b,k,LEN,C0,C1,STRAT,LENSTRAT)

    if c eq 1 then a24,xp,xq,xpq := Explode(C0);
    elif c eq 2 then a24,xp,xq,xpq := Explode(C1); end if;

    if b in [0,1] then X,Z := LADDER_3_pt(b*2^(LEN-1) + k,xp,xq,xpq,a24);
        elif b eq 2 then X,Z := LADDER_3_pt(2*k,xq,xp,xpq,a24); end if;

	pts := [];
	index := 0;
    ii := 1;

    // Treat the first row separately
    // multiply (X:Z) until it has order 4, and store intermediate points
    while index lt LENSTRAT do
        Append(~pts, [X,Z,index]);
        m := STRAT[ii]; ii +:= 1;
        X,Z := xDBLe_affineA24(X,Z,a24,2*m);
        index +:= m;
    end while;

    // compute the 4-isogeny based on kernel (X:Z)
    // evaluate the 4-isogeny at every point in pts
    if X eq Z then
        for i:=1 to #pts do 
            pts[i][1],pts[i][2] := EvalFourIsogenyWithKernelXeqZ(a24,pts[i][1],pts[i][2]);
        end for;
        A24,C24 := GetFourIsogenyWithKernelXeqZ(a24);
    elif X eq -Z then
        for i:=1 to #pts do 
            pts[i][1],pts[i][2] := EvalFourIsogenyWithKernelXeqMinZ(a24,pts[i][1],pts[i][2]);
        end for;
        A24,C24 := GetFourIsogenyWithKernelXeqMinZ(a24);
    else
        A24,C24,K1,K2,K3 := GetFourIsogenyWithKernelXneZ(X,Z); 
        for i:=1 to #pts do 
            pts[i][1],pts[i][2] := EvalFourIsogenyWithKernelXneZ(pts[i][1],pts[i][2],K1,K2,K3); 
        end for;
    end if;

    if LENSTRAT gt 0 then // For very small e

        // R becomes the last point in pts and then pts is pruned
        X := pts[#pts][1]; 
        Z := pts[#pts][2]; 
        index := Integers()!pts[#pts][3];	

        Prune(~pts);

        // Alice's main loop
        for row := 2 to LENSTRAT do
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
    end if;

    A24 := A24 + A24;
    A24 := A24 - C24;
    A24 := A24 + A24;

    j := j_inv(A24,C24);

    return j;
end function;
