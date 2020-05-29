////////////////////////////////////////////////////////////////////////////////
// Magma implementation of Keccak based on the pseudo-code description on 
// http://keccak.noekeon.org/specs_summary.html
// 2017-06-29
//
// Keccak lanes are represented by integers modulo 2^w, where b=25*w is the 
// permutation width. For b=1600=25*64, they are 64-bit integers.
//
// Assumes that messages are input as byte arrays, i.e. their bitlength is 
// divisible by 8.
//
// The code defines the standardized functions 
// SHA3-224, SHA3-256, SHA3-384, SHA3-512,
// SHAKE128, SHAKE256, 
// cSHAKE128, cSHAKE256.
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

// Round constants for maximum lane size 64, truncate for smaller lane sizes.
// Note difference in magma indexing starting at RC[1]. 
RC := [
0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
0x8000000000008080, 0x0000000080000001, 0x8000000080008008
];

// Rotation offsets r[x,y].
// Table from the pseudo-code description.
r_table := [
[25, 39,  3, 10, 43],
[55, 20, 36, 44,  6],
[28, 27,  0,  1, 62],
[56, 14, 18,  2, 61],
[21,  8, 41, 45, 15]
];

r := function(x,y)
    // Magma adjustment for correct table access.
    return r_table[(2-y) mod 5 + 1][(x+2) mod 5 + 1];
end function;

////////////////////////////////////////////////////////////////////////////////
// Bitwise binary operators.
////////////////////////////////////////////////////////////////////////////////

ROT := function(a,d,w)
    // Rotates the integer a mod 2^w by the offset d.
    return BitwiseXor(ShiftLeft(a,d) mod 2^w,ShiftRight(a,w-d)); 
end function;

NOT := function(a,w)
    assert (a ge 0) and (a lt 2^w);
    // Bitwise not of the integer a interpreted as a length-w bit string.
    return 2^w+BitwiseNot(a);
end function;

XOR := function(a,b,w)
    // Simply xor of the bitstrings given by the integers a and b.
    return BitwiseXor(a,b) mod 2^w;
end function;

AND := function(a,b,w)
    // And the bitwise and. 
    return BitwiseAnd(a,b) mod 2^w;
end function;

////////////////////////////////////////////////////////////////////////////////
// Conversion between different formats.
////////////////////////////////////////////////////////////////////////////////

IntToByteSeq := function(a,wbytes)
    // Converts an integer to a byte sequence of fixed length wbytes, 
    // padding with zero bytes.
    aseq := IntegerToSequence(a,2^8);
    for i in [1..(wbytes-#aseq)] do
        Append(~aseq, 0);
    end for;
    return aseq;
end function;

StringToByteArray := function(str)
    // Takes a string and converts it to a byte array character-wise.
    return ElementToSequence(BinaryString(str));
end function;

ByteArrayToHexString := function(M)
    // Converts a byte array into a string of hexadecimal characters. 
    intM := SequenceToInteger(Reverse(M),2^8);
    strM := Sprintf("%h", intM);
    //return Substring(strM,3,#strM-2); // Use this to cut off "0x" at the start.
    return strM;
end function;

////////////////////////////////////////////////////////////////////////////////
// Round function for the Keccak-f permutation. 
////////////////////////////////////////////////////////////////////////////////

round := function(b,A,RCi,w)
    C := [A[x+1][0+1] : x in [0..4]];
    for x in [0..4] do
        for y in [1..4] do
            C[x+1] := XOR(C[x+1],A[x+1][y+1], w);
        end for;
    end for;
    D := [C[((x-1) mod 5)+1] : x in [0..4]];
    for x in [0..4] do
        D[x+1] := XOR(D[x+1], ROT(C[((x+1) mod 5)+1],1,w), w); 
        for y in [0..4] do
            A[x+1,y+1] := XOR(A[x+1,y+1], D[x+1], w);
        end for;
    end for;
    B := [[0,0,0,0,0] : i in [0..4]];    

    for x in [0..4] do
        for y in [0..4] do
            B[y+1][((2*x+3*y) mod 5)+1] := ROT(A[x+1][y+1], r(x,y), w); 
        end for;
    end for;

    for x in [0..4] do
        for y in [0..4] do
            A[x+1][y+1] := XOR(B[x+1,y+1], AND(NOT(B[((x+1) mod 5)+1,y+1],w),B[((x+2) mod 5)+1,y+1],w),w);
        end for;
    end for;

    A[0+1,0+1] := XOR(A[0+1,0+1],RCi,w);

    return A;
end function;

////////////////////////////////////////////////////////////////////////////////
// Keccak-f permutation.
////////////////////////////////////////////////////////////////////////////////

Keccak_f := function(b,A) 
    w := b div 25;
    l := Log(2,w);
    nr := 12+2*l;
    for i in [0..(nr-1)] do
        A := round(b, A, RC[i+1], w);
    end for;

    return A;
end function;

////////////////////////////////////////////////////////////////////////////////
// Keccak sponge construction.
////////////////////////////////////////////////////////////////////////////////

Keccak_slow := function(r, c, M, d, out_byte_len)
    // Assuming M is input as a byte array, 
    // i.e. list of integers mod 2^8 in Magma.  
    b := r+c; // permutation width 
    //assert b in [25,50,100,200,400,800,1600];
    assert b in [200,400,800,1600]; // Make sure we can work with bytes.
    w := b div 25;
    assert w mod 8 eq 0;
    assert r mod w eq 0;
    rbytes := r div 8;
    wbytes := w div 8;
    lenM := #M;
    // Initialization and padding
    S := [[0,0,0,0,0] : i in [0..4]];
    P := Append(M,d); 
    for i in [1..((rbytes - (#P mod rbytes)) mod rbytes)] do
        Append(~P, 0x00);
    end for;  
    P[#P] := BitwiseXor(P[#P], 0x80);
    // Absorbing phase
    for i in [1..(#P div rbytes)] do // go through each r-bit block of P
        // Organize Pi as a list of w-bit values. 
        P_i :=
[SequenceToInteger(P[(((j-1)*wbytes+1)+(i-1)*rbytes)..(j*wbytes+(i-1)*rbytes)],2^8) : j in [1..(r div w)]];
        for x in [0..4] do
          for y in [0..4] do
            if x + 5*y lt (r div w) then
                S[x+1][y+1] := XOR(S[x+1][y+1], P_i[x+5*y+1],w);
            end if;
          end for;
        end for;
        S := Keccak_f(r+c,S);
    end for;

    //Squeezing phase
    Z := [];
    i := 0;
    while i*(r div w)*8 lt out_byte_len do
       for y in [0..4] do
         for x in [0..4] do
            if x + 5*y lt r div w then
                //x + 5*y, S[x+1][y+1];
                Z := Z cat IntToByteSeq(S[x+1][y+1],wbytes);
            end if;
         end for;
       end for;
       S := Keccak_f(r+c,S);
       i +:= 1;
    end while;
 
    return Z;
end function;


////////////////////////////////////////////////////////////////////////////////
// Faster
////////////////////////////////////////////////////////////////////////////////

Keccak := function(r, c, M, d, out_byte_len)
    // Assuming M is input as a byte array, 
    // i.e. list of integers mod 2^8 in Magma.  
    b := r+c; // permutation width 
    //assert b in [25,50,100,200,400,800,1600];
    assert b in [200,400,800,1600]; // Make sure we can work with bytes.
    w := b div 25;
    assert w mod 8 eq 0;
    assert r mod w eq 0;
    rbytes := r div 8;
    wbytes := w div 8;
    lenM := #M;
    // Initialization and padding
    S := [[0,0,0,0,0] : i in [0..4]];
    P := Append(M,d); 
    for i in [1..((rbytes - (#P mod rbytes)) mod rbytes)] do
        Append(~P, 0x00);
    end for;  
    P[#P] := BitwiseXor(P[#P], 0x80);
    // Absorbing phase
    for i in [1..(#P div rbytes)] do // go through each r-bit block of P
        // Organize Pi as a list of w-bit values. 
        P_i :=
[SequenceToInteger(P[(((j-1)*wbytes+1)+(i-1)*rbytes)..(j*wbytes+(i-1)*rbytes)],2^8) : j in [1..(r div w)]];
        for x in [0..4] do
          for y in [0..4] do
            if x + 5*y lt (r div w) then
                S[x+1][y+1] := XOR(S[x+1][y+1], P_i[x+5*y+1],w);
            end if;
          end for;
        end for;
        S := Keccak_f(r+c,S);
    end for;

    //Squeezing phase
    Z := [];
    i := 0;
    while i*(r div w)*8 lt out_byte_len do
       for y in [0..4] do
         for x in [0..4] do
            if x + 5*y lt r div w then
                //x + 5*y, S[x+1][y+1];
                seq := IntToByteSeq(S[x+1][y+1],wbytes);
                for j in [1..#seq] do Append(~Z, seq[j]); end for;
            end if;
         end for;
       end for;
       S := Keccak_f(r+c,S);
       i +:= 1;
    end while;
 
    return Z;
end function;

////////////////////////////////////////////////////////////////////////////////
// Standardized SHA3 functions.
////////////////////////////////////////////////////////////////////////////////

SHAKE128 := function(M, out_byte_len)
    return Keccak(1344,256,M,0x1F,out_byte_len)[1..out_byte_len];
end function;

SHAKE256 := function(M, out_byte_len)
    return Keccak(1088,512,M,0x1F,out_byte_len)[1..out_byte_len];
end function;

SHA3_224 := function(M)
    return Keccak(1152,448,M,0x06,28)[1..28];
end function;

SHA3_256 := function(M)
    return Keccak(1088,512,M,0x06,32)[1..32];
end function;

SHA3_384 := function(M)
    return Keccak(832,768,M,0x06,48)[1..48];
end function;

SHA3_512 := function(M)
    return Keccak(576,1024,M,0x06,64)[1..64];
end function;

////////////////////////////////////////////////////////////////////////////////
// Encoding functions for cSHAKE and KMAC according to NIST.SP.800-185
////////////////////////////////////////////////////////////////////////////////

right_encode := function(x)
    // Section 2.3.1 in NIST.SP.800-185
    // Encodes an integer as a byte string, here in byte sequence format.
    assert 0 le x and x lt 2^2040;
    if x eq 0 then 
        n := 1;
    else
        n := Floor(Log(2,x)/8)+1;
    end if;
    xseq := IntegerToSequence(x,256);
    xseq := xseq cat [0: i in [1..(n-#xseq)]];
    xseq := Reverse(xseq);
    return xseq cat [n];
end function;

left_encode := function(x)
    // Section 2.3.1 in NIST.SP.800-185
    // Encodes an integer as a byte string, here in byte sequence format.
    assert 0 le x and x lt 2^2040;
    if x eq 0 then 
        n := 1;
    else
        n := Floor(Log(2,x)/8)+1;
    end if;
    xseq := IntegerToSequence(x,256);
    xseq := xseq cat [0: i in [1..(n-#xseq)]];
    xseq := Reverse(xseq);
    return [n] cat xseq;
end function;

encode_string := function(S)
    // Section 2.3.2 in NIST.SP.800-185
    // Encodes a bit string by prepending its length with lef_encode.
    // This function only works for character strings, i.e. byte-aligned and
    // output here is in byte sequence format.
    // Note: length of S is length as bit string.
    str := StringToByteArray(S);     
    return left_encode(#S*8) cat str;
end function;

bytepad := function(X,w)
    z := left_encode(w) cat X;
    z := z cat [0: i in [1..(w-#z mod w)]];
    return z;
end function;

left_encode_bitstring := function(x)
    // Section 2.3.1 in NIST.SP.800-185
    // Encodes an integer as a bit string.
    assert 0 le x and x lt 2^2040;
    if x eq 0 then 
        n := 1;
    else
        n := Floor(Log(2,x)/8)+1;
    end if;
    xseq := IntegerToSequence(x,256);
    xseq := xseq cat [0: i in [1..(n-#xseq)]];
    xseq := Reverse(xseq);
    xseq := [n] cat xseq;
    xstr := "";
    for i in [1..#xseq] do
        str_i := Reverse(IntegerToString(2^8 + xseq[i],2))[1..8];
        xstr := xstr cat str_i;
    end for;
    return xstr;
end function;

encode_bitstring := function(S)
    Senc := left_encode_bitstring(#S) cat S;
    Sbytes := [];
    for i in [1..(#Senc div 8)] do
        Append(~Sbytes, StringToInteger(Reverse(Senc[((i-1)*8+1)..(i*8)]),2));
    end for;
    return Sbytes;
end function;

encode_bytearray := function(A)
    return left_encode(#A*8) cat A;
end function;

////////////////////////////////////////////////////////////////////////////////
// cSHAKE according to NIST.SP.800-185
////////////////////////////////////////////////////////////////////////////////


cSHAKE128 := function(X,L,N,S)
    assert L mod 8 eq 0; // only correct output here if length in bits is divisible by 8 
    if L eq 0 then return []; end if;
    if N eq "" and S eq "" then
        return SHAKE128(X,L);
    else
        out_bytes := L div 8;
        M := bytepad(encode_string(N) cat encode_string(S), 168) cat X;
        return Keccak(1344,256,M,0x04,out_bytes)[1..out_bytes];
    end if;
end function;

cSHAKE256 := function(X,L,N,S)
    assert L mod 8 eq 0; // only correct output here if length in bits is divisible by 8 
    if L eq 0 then return []; end if;
    if N eq "" and S eq "" then
        return SHAKE256(X,L);
    else
        out_bytes := L div 8;
        M := bytepad(encode_string(N) cat encode_string(S), 136) cat X;
        return Keccak(1088,512,M,0x04,out_bytes)[1..out_bytes];
    end if;
end function;

////////////////////////////////////////////////////////////////////////////////
// KMAC according to NIST.SP.800-185
////////////////////////////////////////////////////////////////////////////////

KMAC128 := function(K,X,L,S)
    // Note K is input as a byte array, here a list of integers.
    newX := bytepad(encode_bytearray(K), 168) cat X cat right_encode(L);
    return cSHAKE128(newX,L,"KMAC",S);
end function;

KMAC256 := function(K,X,L,S)
    // Note K is input as a byte array, here a list of integers.
    newX := bytepad(encode_bytearray(K), 136) cat X cat right_encode(L);
    return cSHAKE256(newX,L,"KMAC",S);
end function;


