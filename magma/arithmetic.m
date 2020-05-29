// Taken from spec Magma
j_inv:=function(A,C); 

	/*
	Computes the j-invariant of a Montgomery curve with projective constant.

	Input:  
    - The projective curve constant (A:C) given by A,C in Fp2.

	Output: 
    - The j-invariant j=256*(A^2-3*C^2)^3/(C^4*(A^2-4*C^2)) of the Montgomery 
      curve B*y^2=x^3+(A/C)*x^2+x or (equivalently) the j-invariant of 
      B'*y^2=C*x^3+A*x^2+C*x.
	*/


	jinv:=A^2;		
	t1:=C^2;
	t0:=t1+t1;
	t0:=jinv-t0;
	t0:=t0-t1;
	jinv:=t0-t1;
	t1:=t1^2;
	jinv:=jinv*t1;
	t0:=t0+t0;
	t0:=t0+t0;
	t1:=t0^2;
	t0:=t0*t1;
	t0:=t0+t0;
	t0:=t0+t0;
	jinv:=1/jinv; 
	jinv:=t0*jinv;

	return jinv;		// Total: 3M+4S+8a+1I

end function;

// Taken from spec Magma
function xDBL(X,Z,A24,C24)
    t0 := X-Z;
    t1 := X+Z;
    t0 := t0^2;
    t1 := t1^2;
    Z2 := C24*t0;
    X2 := Z2*t1;
    t1 := t1-t0;
    t0 := A24*t1;
    Z2 := Z2+t0;
    Z2 := Z2*t1;

    return X2,Z2;
end function;

// Taken from spec Magma
function xDBLe(X,Z,A24,C24,e)
    X_ := X; Z_ := Z;

    for i in [1..e] do
        X_,Z_ := xDBL(X_,Z_,A24,C24);
    end for;

    return X_,Z_;
end function;

function xDBL_affineA24(X,Z,a24)
    t0 := X-Z;
    t1 := X+Z;
    t0 := t0^2;
    t1 := t1^2;
    X2 := t0*t1;
    t1 := t1-t0;
    Z2 := a24*t1;
    Z2 := t0+Z2;
    Z2 := Z2*t1;

    return X2,Z2;
end function;

function xDBLe_affineA24(X,Z,a24,e)
    X_ := X; Z_ := Z;

    for i in [1..e] do
        X_,Z_ := xDBL_affineA24(X_,Z_,a24);
    end for;

    return X_,Z_;
end function;

// Taken from spec Magma
xDBLADD:=function(XP,ZP,XQ,ZQ,xPQ,A24)

	/*
	Carries out a typical step in the Montgomery ladder: a simultaneous 
    doubling and differential addition.
	
    Input: 
    - The projective Montgomery x-coordinates of xP=XP/ZP and xQ=XQ/ZQ, 
    - the affine difference x(P-Q) and 
    - the Montgomery curve constant A24=(A+2)/4.

	Output: 
    - The projective Montgomery x-coordinates of x(2P)=X2P/Z2P and
      x(Q+P)=XQP/ZQP.
	*/
        
    t0:= XP+ZP;
    t1:= XP-ZP;
    X2P:= t0^2;
    t2:= XQ-ZQ;
    XQP:= XQ+ZQ;
    t0:= t0*t2;
    Z2P:= t1^2;
    t1:= t1*XQP;
    t2:=X2P-Z2P;
    X2P:= X2P*Z2P;
    XQP:=A24*t2;
    ZQP:= t0-t1;
    Z2P:=XQP+Z2P;
    XQP:=t0+t1;
    Z2P:= Z2P*t2;
    ZQP:=ZQP^2;
    XQP:=XQP^2;
    ZQP:=xPQ*ZQP;
    
    return X2P,Z2P,XQP,ZQP;		// Total: 6M+4S+8a

end function;

// Taken from spec Magma
LADDER_3_pt:=function(m,xP,xQ,xPQ,A24)

	bits:=IntegerToSequence(m,2);
    
	R0:=xQ; Z0:=Parent(xQ)!1;
	R1:=xP; Z1:=Parent(xP)!1;
	R2:=xPQ; Z2:=Parent(xPQ)!1;

	for i:=1 to #bits do
		if bits[i] eq 1 then
			R0,Z0,R1,Z1:=xDBLADD(R0,Z0,R1,Z1,R2,A24);
			R1:=R1*Z2;
		else
			R0,Z0,R2,Z2:=xDBLADD(R0,Z0,R2,Z2,R1,A24);
			R2:=R2*Z1;
		end if;
	end for;

	return R1,Z1;
	
end function;

function GetFourIsogenyWithKernelXeqZ(a24)
    A24 := a24;
    C24 := a24-1;

    return A24,C24;
end function;

function GetFourIsogenyWithKernelXeqMinZ(a24)
    A24 := a24-1;
    C24 := a24;

    return A24,C24;
end function;

function EvalFourIsogenyWithKernelXeqZ(a24,X,Z)
    //X_ := (X+Z)^2*((X+Z)^2-4*(1-a24)*X*Z);
    //Z_ := 4*(1-a24)*X*Z*(X-Z)^2;
   
    T0 := X+Z;  
    T2 := X-Z;
    T0 := T0^2;
    T2 := T2^2;
    T1 := 1-a24;
    T1 := T1+T1;
    T1 := T1+T1;
    T1 := T1*X;
    T1 := T1*Z;
    Z := T1*T2;
    T1 := T0-T1;
    X := T0*T1;

    return X,Z; // 4M+2S+3a+3s
end function;

function EvalFourIsogenyWithKernelXeqMinZ(a24,X,Z)
    //X_ := (X-Z)^2*((X-Z)^2+4*a24*X*Z);
    //Z_ := -4*a24*X*Z*(X+Z)^2;
        
    T2 := X+Z; 
    T0 := X-Z;
    T2 := T2^2;
    T0 := T0^2;
    T1 := a24+a24;
    T1 := T1+T1;
    T1 := T1*X;
    T1 := T1*Z;
    T2 := T1*T2;
    Z := -T2;
    T1 := T0+T1;
    X := T0*T1;

    return X,Z; // 4M+2S+4a+2s
end function;

function GetFourIsogenyWithKernelXneZ(X4,Z4)
    //K1 := Z4^2;
    //R1 := X4^2;
    //R1 := R1^2;
    //C24 := K1^2;
    //A24 := R1-C24;
    //K1 := 4*K1;
    //K2 := X4-Z4;
    //K3 := X4+Z4;
    //A24 := A24+C24;
    
    //The below is the SIKE C, the above SIKE Magma..
    
    K2 := X4-Z4;
    K3 := X4+Z4;
    K1 := Z4^2;
    K1 := K1+K1;
    C24 := K1^2;
    K1 := K1+K1;
    A24 := X4^2;
    A24 := A24+A24;
    A24 := A24^2;

    return A24,C24,K1,K2,K3;
end function;

function EvalFourIsogenyWithKernelXneZ(X,Z,K1,K2,K3)
    //X_ := X*(2*X4*Z4*Z-X*(X4^2+Z4^2))*(X4*X-Z4*Z)^2;
    //Z_ := Z*(2*X4*Z4*X-Z*(X4^2+Z4^2))*(Z4*X-X4*Z)^2;

    //return X_,Z_;

    //R2 := X+Z; 
    //R3 := R2*K2; 
    //R4 := X-Z;
    //R1 := R4*K3; 
    //R2 := R4*R2; 
    //R4 := R1+R3;
    //R4 := R4^2; 
    //R3 := R1-R3;
    //R3 := R3^2;
    //R2 := K1*R2; 
    //R1 := R4+R2;
    //R2 := R3-R2;
    //X_ := R4*R1;
    //Z_ := R3*R2;					// Total: 6M+2S+6a
    
    //The below is the SIKE C, the above SIKE Magma..
        
    T0 := X+Z;
    T1 := X-Z;
    X := T0*K2;
    Z := T1*K3;
    T0 := T0*T1;
    T0 := T0*K1;
    T1 := X+Z;
    Z := X-Z;
    T1 := T1^2;
    Z := Z^2;
    X := T1+T0;
    T0 := Z-T0;
    X := X*T1;
    Z := Z*T0;

	return X,Z;	
end function;

function GetTwoIsogenyWithKernelXneZ(X,Z)
    X := X^2;
    Z := Z^2;
    X := Z-X;

    return X,Z;
end function;

function EvalTwoIsogenyWithKernelXneZ(XP,ZP,X,Z)
    T0 := X+Z;
    T1 := ZP-XP;
    T0 := T0*T1;
    T1 := X-Z;
    T2 := XP+ZP;
    T1 := T1*T2;
    T2 := T1-T0;
    T1 := T1+T0;
    X := X*T2;
    Z := Z*T1;

    return X,Z;
end function;

function SimpleIsogeny(A,P,Q,e,b,k)

    if b in [0,1] then R := P + (b*2^(e-1) + k)*Q;
        elif b eq 2 then R := 2*k*P + Q; end if;

    X := R[1]; Z := 1; // Change
    A24 := (A+2)/4; C24 := 1; // Change

    X_,Z_ := xDBLe(X,Z,A24,C24,e-2); // First step separately
    if X_ eq Z_ then
        X,Z := EvalFourIsogenyWithKernelXeqZ(A24,X,Z);
        A24,C24 := GetFourIsogenyWithKernelXeqZ(A24);
    elif X_ eq -Z_ then
        X,Z := EvalFourIsogenyWithKernelXeqMinZ(A24,X,Z);
        A24,C24 := GetFourIsogenyWithKernelXeqMinZ(A24);
    else
        A24,C24,K1,K2,K3 := GetFourIsogenyWithKernelXneZ(X_,Z_);
        X,Z := EvalFourIsogenyWithKernelXneZ(X,Z,K1,K2,K3);
    end if;

    for i in [2..e div 2] do
        X_,Z_ := xDBLe(X,Z,A24,C24,e-2*i);
        A24,C24,K1,K2,K3 := GetFourIsogenyWithKernelXneZ(X_,Z_);
        X,Z := EvalFourIsogenyWithKernelXneZ(X,Z,K1,K2,K3);
    end for;

    a24 := A24/C24; a := 4*a24-2;

    return a;
end function;
