function TwoDDL(T,P,Q,e2)
    lambda := 0;
    mu := 0;
    P0 := 2^(e2-1)*P;
    Q0 := 2^(e2-1)*Q;
    E0 := Parent(P);

    for ii in [0..e2-1] do
        assert 2^(e2-ii)*T eq E0!0;

        S := 2^(e2-ii-1)*T;

        for jj in [0,1] do
        for kk in [0,1] do
            if S eq jj*P0 + kk*Q0 then
                lambda +:= jj*2^ii;
                mu +:= kk*2^ii;

                T -:= jj*2^ii*P;
                T -:= kk*2^ii*Q;
            end if;
        end for;
        end for;
    end for;

    return lambda,mu;
end function;

function FindPreimage(x,A)
    Fp2 := Parent(A);
    FF<X> := PolynomialRing(Fp2);

    f := 4*X*(X^2+A*X+1)*x - (X^4 + (A-6)*X^3 + (34-6*A)*X^2 + 1);

    return Roots(f);
end function;

function GetSubPoint(e2,A,params)
    Fp2 := Parent(A);
    x := Fp2!0;
    p := 2^params`e2*3^params`e3*params`f-1;
    for i in [0..e2-3] do
        pre := FindPreimage(x,A);
        for j in [1..#pre] do
            z := pre[j][1];
            w := FindPreimage(z,A);
            for k in [1..4] do
                if w[1][1]^p eq w[1][1] and w[2][1]^p eq w[2][1] and w[3][1]^p eq w[3][1] and w[4][1]^p eq w[4][1] then
                    x := z;
                end if;
            end for;
        end for;
        assert x^p eq x;

        y := Sqrt(x^3+A*x^2+x);
        E := EllipticCurve([Fp2|0,A,0,1,0]);
        assert 2^(i+1)*E![x,y] eq E![0,0];
        assert 2^(i+2)*E![x,y] eq E!0;
    end for;

    pre := FindPreimage(x,A);
    x := pre[1][1];

    return x;
end function;

function SimplestIsogeny(A,P,Q,e,k,params)
    R := P + k*Q;

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
    Fp2 := Parent(a);
    E := EllipticCurve([Fp2|0,a,0,1,0]);

    P := Random(E);
    assert 2^params`e2*3^params`e3*params`f*P eq E!0;

    return jInvariant(E);
end function;

function GetBasis(E2,E3,f,e2,Fp2,params)
    A := Fp2!6;
    E0 := EllipticCurve([Fp2|0,A,0,1,0]);
    p := 2^params`e2*3^params`e3*params`f-1;
    pi := map< E0 -> E0 | T :-> [T[1]^p,T[2]^p] >;

    x := GetSubPoint(e2,A,params);
    y := Sqrt(x^3 + A*x^2 + x);
    Q := E0![x,y];

    assert 2^(e2-1)*Q eq E0![0,0];
    assert 2^e2*Q eq E0!0;

    repeat
        P0 := f*Random(E0);
        P1 := 3^E3*P0;
        P2 := 2^(E2-1)*P1;
    until P2 ne E0!0 and P2 ne E0![0,0] and 2*P2 eq E0!0;
    P := 2^(E2-e2)*P1;

    assert 2^(e2-1)*P ne E0!0 and 2^(e2-1)*Q ne E0!0;
    assert 2^e2*P eq E0!0 and 2^e2*Q eq E0!0;
    assert 2^(e2-1)*P ne E0![0,0] and 2^(e2-1)*Q eq E0![0,0];

    alpha,beta := TwoDDL(pi(Q),P,Q,e2);
    assert alpha*P + beta*Q eq pi(Q);
    assert alpha eq 0;
    assert beta eq 1;

    lambda,mu := TwoDDL(pi(P),P,Q,e2);
    assert lambda eq 2^e2-1;
    assert IsEven(mu);
    assert -P + mu*Q eq pi(P);

    P := P - (mu div 2)*Q;

    assert 2^(e2-1)*P ne E0!0 and 2^(e2-1)*Q ne E0!0;
    assert 2^e2*P eq E0!0 and 2^e2*Q eq E0!0;
    assert 2^(e2-1)*P ne E0![0,0] and 2^(e2-1)*Q eq E0![0,0];
    assert pi(P) eq -P;
    assert pi(Q) eq Q;

    return [(Fp2!A+2)/4,P[1],Q[1],(P+Q)[1]];
end function;

//clear;
//
//A := 6;
//QQ := Rationals();
//RR<alpha,beta> := PolynomialRing(QQ,2);
//RR<alpha,beta> := quo< RR | beta^2 - alpha^3 - A*alpha^2 - alpha >;
//kk<alpha,beta> := FieldOfFractions(RR);
//
//EE := EllipticCurve([kk|0,A,0,1,0]);
//PP := EE![alpha,beta];
//assert 4*alpha*(alpha^2+A*alpha+1)*(2*PP)[1] eq (alpha^4 + (A-6)*alpha^3 + (34-6*A)*alpha^2 + 1);

//
//clear;
//
//QQ := Rationals();
//RR<alpha,beta> := PolynomialRing(QQ,2);
//RR<alpha,beta> := quo< RR | beta^2 - alpha^3 + alpha >;
//kk<alpha,beta> := FieldOfFractions(RR);
//
//EE := EllipticCurve([kk|-1,0]);
//PP := EE![alpha,beta];
//assert 4*(alpha^2-1)*alpha*(2*PP)[1] eq (alpha^2+1)^2;
