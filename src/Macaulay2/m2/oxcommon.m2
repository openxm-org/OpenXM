ID = "$OpenXM$"
--- class INT32
INT32 = new SelfInitializingType of BasicList
new INT32 from ZZ := (a,n) -> {n}
int32 = method()
int32(INT32) := (n)->(n)
int32(ZZ) := (n)-> (
     if ((abs(n) >> 31) > 0) then error "expected signed 32-bit integer";
     tmp := n;
     if (n < 0) then tmp = 2^32+n; -- 2's complement
     new INT32 from tmp
	 )

--- class ERROR, MATHCAP
ERROR = new SelfInitializingType of BasicList

-- class MATHCAP
MATHCAP = new SelfInitializingType of BasicList

MATHCAP'OF'M2 = MATHCAP
  {{INT32{1001003}, "M2 client " | ID}, 
       {},
       {{INT32{OX'DATA}, 
         {INT32{CMO'NULL}, INT32{CMO'INT32}, INT32{CMO'STRING}, 
          INT32{CMO'LIST}, INT32{CMO'ERROR2}, INT32{CMO'MATHCAP}, 
	  INT32{CMO'ZZ}}
     }}
}

MATHCAP'OF'M2'SERVER = MATHCAP
  {{INT32{1001003}, "M2 client " | ID}, 
       {INT32{SM'popCMO},
	INT32{SM'popString},
	INT32{SM'pops},
	INT32{SM'nop},
	INT32{SM'setName},
	INT32{SM'evalName},
	INT32{SM'executeStringByLocalParser},
	INT32{SM'executeFunction},
	INT32{SM'dupErrors},
	INT32{SM'setMathCap},
	INT32{SM'mathcap}
	},
       {{INT32{OX'DATA}, 
         {INT32{CMO'NULL}, INT32{CMO'INT32}, INT32{CMO'STRING}, 
          INT32{CMO'LIST}, INT32{CMO'ERROR2}, INT32{CMO'MATHCAP}, 
	  INT32{CMO'ZZ}}
     }}
}


--- class OXSESSION
OXSESSION = new Type of MutableHashTable;

toNetwork = (n) -> (
     n3 := n % 256;
     n = n >> 8;
     n2 := n % 256;
     n = n >> 8;
     n1 := n % 256;
     n0 := n >> 8;
     (ascii n0) | (ascii n1) | (ascii n2) | (ascii n3))

fromNetwork = (s) -> (
     s = ascii substring(s,0,4);
     n := s#0;
     n = (n << 8) + s#1;
     n = (n << 8) + s#2;
     n = (n << 8) + s#3;
     n)


OXCOMMAND = toNetwork 513
OXDATA = toNetwork 514

OX'COMMAND = 513
OX'DATA = 514

CMO'NULL = 1
CMO'INT32 = 2
CMO'STRING = 4
CMO'LIST = 17
CMO'ZZ = 20
CMO'QQ = 21
CMO'ERROR2 = 2130706432 + 2
CMO'MATHCAP = 5
CMO'RECURSIVE'POLYNOMIAL = 27
CMO'POLYNOMIAL'IN'ONE'VARIABLE = 33
CMO'INDETERMINATE = 60

CMONULL = toNetwork CMO'NULL
CMOINT32 = toNetwork CMO'INT32
CMOSTRING = toNetwork CMO'STRING
CMOLIST = toNetwork CMO'LIST
CMOZZ = toNetwork CMO'ZZ
CMOQQ = toNetwork CMO'QQ
CMOERROR2 = toNetwork CMO'ERROR2
CMOMATHCAP = toNetwork CMO'MATHCAP
CMORECURSIVEPOLYNOMIAL = toNetwork CMO'RECURSIVE'POLYNOMIAL
CMOPOLYNOMIALINONEVARIABLE = toNetwork CMO'POLYNOMIAL'IN'ONE'VARIABLE
CMOINDETERMINATE = toNetwork CMO'INDETERMINATE

-- Defines taken from the OpenXM documentation
-------------------
-- Stack machine --
-------------------
SM'popSerializedLocalObject = 258
SM'popCMO = 262
SM'popString = 263 

SM'mathcap = 264
SM'pops = 265
SM'setName = 266
SM'evalName = 267 
SM'executeStringByLocalParser = 268 
SM'executeFunction = 269
SM'beginBlock  = 270
SM'endBlock    = 271
SM'shutdown    = 272
SM'setMathCap  = 273
SM'executeStringByLocalParserInBatchMode = 274
SM'getsp   = 275
SM'dupErrors = 276
SM'pushCMOtag = 277

SM'control'kill = 1024
SM'control'reset'connection  = 1030
SM'control'to'debug'mode = 1025
SM'control'exit'debug'mode = 1026

--------------------------------
-- binary string --> CMObject --
--------------------------------

OXINPUT = new Type of MutableHashTable
makeOXINPUT = (F) -> (
     result := new OXINPUT;
     result.F = F;
     result.buffer = "";
     result.loc = 0;
     result)

readMoreData = (OX) -> (
     -- OX is an OXINPUT
     newstring := read (OX.F);
     stderr << "Read " << #newstring << " bytes: " << ascii newstring 
            << endl << flush;
     left := #OX.buffer - OX.loc;
     if left > 0 then
        OX.buffer = substring(OX.buffer, OX.loc, left)
                    | newstring
     else
        OX.buffer = newstring;
     OX.loc = 0;
     )

get32bits = (OX) -> (
     -- OX is an OXINPUT
     if #OX.buffer < OX.loc + 4
       then readMoreData OX;
     result := fromNetwork(substring(OX.buffer, OX.loc, 4));
     OX.loc = OX.loc + 4;
     result
     )

getBytes = (OX,n) -> (
     -- OX is an OXINPUT
     if #OX.buffer < OX.loc + n
       then readMoreData OX;
     result := substring(OX.buffer, OX.loc, n);
     OX.loc = OX.loc + n;
     result)
     
CMOfromBinary = (OX) -> (
     -- OX is an OXINPUT
     local result;
     cmoTag := get32bits OX;
     if cmoTag === CMO'NULL then 
         null
     else if cmoTag === CMO'INT32 then (
	  -- next 4 bytes are an integer
	  result = get32bits OX;
	  if (result > 2^31 - 1) then result = result - 2^32;	  
	  INT32 result)
     else if cmoTag === CMO'STRING then (
     	  len = get32bits OX;
	  getBytes(OX,len))
     else if cmoTag === CMO'LIST then (
     	  len = get32bits OX;
     	  apply(len, i -> CMOfromBinary OX))
     else if cmoTag === CMO'ERROR2 then (
	  CMOfromBinary OX)
     else if cmoTag === CMO'ZZ then (
	  len = get32bits OX;
	  if (len > 2^31-1) then len = len - 2^32;
	  result = 0;
	  i = 0;
	  while (i < abs(len)) do (
	       result = result + ( (get32bits OX) << (32*i) );
	       i = i+1;
	       );
	  if (len < 0) then result = -result;
	  result
	  )
     else if cmoTag === CMO'QQ then (
	  -- make the numerator
	  len = get32bits OX;
	  if (len > 2^31-1) then len = len - 2^32;
	  num := 0;
	  i := 0;
	  while (i < abs(len)) do (
	       num = num + ( (get32bits OX) << (32*i) );
	       i = i+1;
	       );
	  if (len < 0) then num = -num;
	  -- make the denominator
	  len = get32bits OX;
	  if (len > 2^31-1) then len = len - 2^32;
	  den := 0;
	  i = 0;
	  while (i < abs(len)) do (
	       den = den + ( (get32bits OX) << (32*i) );
	       i = i+1;
	       );
	  if (len < 0) then den = -den;
	  num/den
	  )
     else if cmoTag === CMO'MATHCAP then (
	  MATHCAP(CMOfromBinary OX)
	  )
     else if cmoTag === CMO'RECURSIVE'POLYNOMIAL then (
	  -- recursive'varlist is a global variable for now
	  -- otherwise, we need way to pass it as a parameter in recursion
     	  recursive'varlist = apply(CMOfromBinary OX, i -> value i);
     	  CMOfromBinary OX
	  )
     else if cmoTag === CMO'POLYNOMIAL'IN'ONE'VARIABLE then (
	  m := get32bits(OX);
	  v := get32bits(OX);
	  i = 0;
	  result = 0;
	  while (i < m) do (
	       exponent := get32bits(OX);
	       result=result+(recursive'varlist_v)^exponent*CMOfromBinary(OX);
	       i = i+1;
	       );
	  result
	  )
     else if cmoTag === CMO'INDETERMINATE then (
	  CMOfromBinary OX
	  )
     else error "unknown CMO tag"
     )

---------------------------
-- CMObject --> binary ----
---------------------------
CMOSerialize = method()

CMOSerialize(File,List) := (F,x) -> (
     F << CMOLIST << toNetwork(#x);
     scan(x, a -> CMOSerialize(F,a)))

CMOSerialize(File,ZZ) := (F,x) -> (
     -- BUG: write this
     temp := abs(x);
     stringform := "";
     i := 0;
     while (temp != 0) do (
	  stringform = stringform | toNetwork(temp % (2^32));
	  temp = temp >> 32;
	  i = i+1;
	  );
     if (i > 2^31-1) then error "integer too large to push";
     if (x < 0) then stringform = toNetwork(2^32-i) | stringform
     else stringform = toNetwork(i) | stringform;
     F << CMOZZ << stringform;
     )

CMOSerialize(File,QQ) := (F,x) -> (
     -- BUG: write this
     temp := abs(numerator x);
     numstring := "";
     i := 0;
     while (temp != 0) do (
	  numstring = numstring | toNetwork(temp % (2^32));
	  temp = temp >> 32;
	  i = i+1;
	  );
     if (i > 2^31-1) then error "number too large to push";
     if (numerator x < 0) then numstring = toNetwork(2^32-i) | numstring
     else numstring = toNetwork(i) | numstring;
     temp = abs(denominator x);
     denstring := "";
     i = 0;
     while (temp != 0) do (
	  denstring = denstring | toNetwork(temp % (2^32));
	  temp = temp >> 32;
	  i = i+1;
	  );
     if (i > 2^31-1) then error "number too large to push";
     denstring = toNetwork(i) | denstring;
     F << CMOQQ << numstring << denstring;
     )

CMOSerialize(File,INT32) := (F,x) -> (
     -- Recall that INT32 handles 2-complemented numbers already.
     F << CMOINT32 << toNetwork(x#0);
     )

CMOSerialize(File,String) := (F,x) -> (
     F << CMOSTRING << toNetwork(#x) << x;
     )

CMOSerialize(File,Nothing) := (F,x) -> (
     F << CMONULL;
     )

CMOSerialize(File,ERROR) := (F,x) -> (
     F << CMOERROR2;
     CMOSerialize(F,toList x);
     )

CMOSerialize(File,MATHCAP) := (F,x) -> (
     F << CMOMATHCAP;
     CMOSerialize(x#0);
     )

CMOSerialize(File, RingElement) := (F, x) -> (
     -- make list of indeterminates
     L := apply(generators ring x, i -> toString i);
     L = apply(L, i -> (CMOINDETERMINATE | CMOSTRING | toNetwork(#i) | i));
     L = CMOLIST | toNetwork(#L) | demark("", L);
     F << CMORECURSIVEPOLYNOMIAL << L;
     -- bug: if 1_R is pushed, then 1_ZZ is popped
     if (sum mingle exponents x == 0) then (F << CMOPOLYNOMIALINONEVARIABLE 
     	  << toNetwork(1) << toNetwork(0) << toNetwork(0));
     CMOSerializePolyInOneVar(F, x);
     )

CMOSerializePolyInOneVar = (F, f) -> (
     local tmp;
     local allVars;
     -- number of variables appearing in f
     allVars = positions(sum exponents f, i -> i > 0);
     if (allVars === {}) then (
	  CMOSerialize(F, substitute(f, coefficientRing ring f));
	  )
     else (
	  -- given a polynomial with only one variable x appearing, 
	  -- make list of sorted pairs where the first element is 
	  -- a monomial in x and the second is the coefficient
	  tmp = coefficients({allVars#0}, f);
	  tmp = sort pack(2, mingle {(entries tmp#0)#0, (entries tmp#1)#0});
	  j := 0;
	  F << CMOPOLYNOMIALINONEVARIABLE << toNetwork(#tmp) << 
	  toNetwork(allVars#0);
	  if (#allVars == 1) then (
	       -- if f is a polynomial of a single variable
	       while (j < #tmp) do (
		    F << toNetwork(sum mingle exponents tmp#j#0);
		    CMOSerialize(F, substitute(tmp#j#1, coefficientRing ring tmp#j#1));
		    j = j+1;
		    );
	       )
	  else (
	       -- otherwise need to do recursion
	       while(j < #tmp) do (
		    F << toNetwork(sum mingle exponents (tmp#j#0));
		    CMOSerializePolyInOneVar(F, tmp#j#1);
		    j = j+1;
		    );
	       );
	  );
     )
