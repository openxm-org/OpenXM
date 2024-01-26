/* $OpenXM: OpenXM/src/kan96xx/Kan/usage.c,v 1.51 2018/09/07 00:09:32 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"

/* A list of undocumented commands for debugging.

  (red@) (debug) switch_function
*/

void Kusage(ob)
     struct object ob;
{
  char *s;
  printf("\n");
  if (ob.tag != Sdollar) {
    printf("Type in ? in order to see system dictionary.\n");
    printf("$keyword$ usage  ; \n");
  }else {
    s = ob.lc.str;
    Kusage2(stdout,s);
  }

}

#define fppp fprintf

void Kusage2(fp,s)
     FILE *fp;
     char *s;
{
  /*int n,i; fprintf(stderr,"\n%d: ",strlen(s));
    for (i=0; i<strlen(s); i++) fprintf(stderr," %x",s[i]);*/
  if (strcmp(s,"add")==0) {
    fppp(fp,"<< obj1 obj2 add obj3 >>\n");
    fppp(fp,"obj3 is the sum of obj1 and obj2.\n");
    fppp(fp,"If obj1=f1/f2, obj2=g1/g2, then obj3 is given by (g2 f1 + f2 g1)/(f2 g2)\n");
    fppp(fp,"Example:  2 3 add ::  ==> 5\n");
  } else if (strcmp(s,"[")==0 || strcmp(s,"]")==0) {
    fppp(fp,"[ and ] are used to construct an array.\n");
    fppp(fp,"The left bracket [ is an operator that leaves an object called\n");
    fppp(fp,"a mark on the stack. The interpreter puts more objects\n");
    fppp(fp,"until it encounters a right bracket, which creates\n");
    fppp(fp,"an array. So, it is possible to construct an array, for example, in the way\n");
    fppp(fp," [  1  2  [ 2 1 roll 3] ] print ==> [1 [2 3]]\n");
  } else if (strcmp(s,"aload")==0) {
    fppp(fp,"<< [f1 f2 ... fn] aload f1 f2 ... fn [f1 ... fn] >>\n");
    fppp(fp,"The primitive aload decomposes an array into the set of the elements.\n");
    fppp(fp,"Example: [(x1+1) (23)] aload length ===> (x1+1) (23) 2\n");
  }else if (strcmp(s,"lt")==0 || strcmp(s,"gt")==0 || strcmp(s,"eq")==0) {
    fppp(fp,"<< o1 o2 lt r1  >> or  <<  o1 o2 gt r1 >> or <<  o1 o2 gt r1 >>\n");
    fppp(fp,"integer r1;\n");
    fppp(fp,"These premitives compare objects o1 and o2.\n");
    fppp(fp,"[lt (less than), gt (greater than), eq (equal)]\n");
    fppp(fp,"The result is r1 which is integer object.\n");
    fppp(fp,"Here, 1 means true and 0 means false.\n");
    fppp(fp,"Example:  (abc) (bca) eq ::  ===> 0\n");
  }else if (strcmp(s,"bye")==0 || strcmp(s,"QUIT")==0 || strcmp(s,"quit")==0) {
    fppp(fp,"The bye or QUIT or quit statement terminates the kan/sm1.\n");
  }else if (strcmp(s,"cat_n")==0) {
    fppp(fp,"<< s1 s2 ... sn n cat_n s1s2...sn >>\n");
    fppp(fp,"string s1, s2, ..., sn ; integer n ; string s1s2...sn\n");
    fppp(fp,"The primitive cat_n concatnates the strings(dollars) s1 ... sn.\n");
    fppp(fp,"Example: (ab c) (2) 2 cat_n ===> (ab c2)\n"); /*\hrule*/
  }else if (strcmp(s,"cclass") == 0) {
    fppp(fp,"<< [super-class-tag, o1, o2, ...] size [class-tag] cclass [class-tag, o1, o2, .... ] >>\n");
  }else if (strcmp(s,"closefile")==0) {
    fppp(fp,"<< fd closefile >>\n");
    fppp(fp,"file fd;\n");
    fppp(fp,"cf. file (open a file)\n");
  }else if (strcmp(s,"coefficients")==0 || strcmp(s,"coeff")==0) {
    fppp(fp,"<< f v coeff [exponents coefficients] >>\n");
    fppp(fp,"poly f,v; list of integers exponents; list of poly coefficients;\n");
    fppp(fp,"The primitive coeff returns the array of exponents and\n");
    fppp(fp,"the array of coefficients of the polynomial f with respect to\n");
    fppp(fp,"the variable v.\n");
    fppp(fp,"The primitive coefficients have not yet removed for a compatibility with the old macro packages.\n");
    fppp(fp,"Note that the data type of each element of exponents is <Poly>(9) and it may cause a trouble when p>0.\n");
    fppp(fp,"cf. dc,  (interger) dc\n");
    fppp(fp,"C-functions: Kparts2, parts2\n");
    fppp(fp,"Example: [(x,y) ring_of_polynomials [[(x) 1 (y) 1]] weight_vector\n");
    fppp(fp,"           0] define_ring\n");
    fppp(fp,"         (x^2+ x y + x). (x). coeff print ;\n");
    fppp(fp,"  ===>  [ [ 2 , 1 ] , [1 , y+1] ]\n");
  }else if (strcmp(s,"copy")==0) {
    fppp(fp,"<< f1 f2 ... fn n copy f1 f2 ... fn f1 f2 ... fn >>\n");
    fppp(fp,"The primitive copy_n duplicates the objects f1 f2 ... fn.\n");
    fppp(fp,"integer n;\n");
    fppp(fp,"Example: (x1+1) 1 copy ===> (x1+1) (x1+1)\n");
    fppp(fp,"<<copy>> copies the values of the objects.  cf. dup\n");
  }else if (strcmp(s,"data_conversion")==0) {
    fppp(fp,"<< obj (type?) data_conversion type >>\n");
    fppp(fp,"<< obj (type??) data_conversion type >>\n");
    fppp(fp,"<< obj (key word) data_conversion obj >>\n");
    fppp(fp,"integer type; object obj;\n");
    fppp(fp,"Key words: null, integer, literal, dollar, string, poly, universalNumber,\n");
    fppp(fp,"           ring, numerator, denominator, double, cancel,\n");
    fppp(fp,"           map, orderMatrix, error, \n");
    fppp(fp," or [(class) (class-name)].\n");
    fppp(fp,"The primitive reports and converts the data type of obj.\n");
    fppp(fp,"Special combinations of obj and keyword.\n");
    fppp(fp,"   ring (oxRingStructure) data_conversion list\n");
    fppp(fp,"Example: (2) (integer) data_conversion ===> 2\n");
  }else if (strcmp(s,"def")==0 || strcmp(s,"set")==0) {
    fppp(fp,"<< /name object def >>  or  << object /name set >>\n");
    fppp(fp,"The primitive def or set binds the object to the name\n");
    fppp(fp,"Example: /abc (23) def    abc :: ---> (23)\n");
    fppp(fp,"Example: (23) /abc set    abc :: ---> (23)\n");
  }else if (strcmp(s,"degree")==0) {
    fppp(fp,"<< f  v  degree  deg >>\n");
    fppp(fp,"poly f,v; integer deg;\n");
    fppp(fp,"deg is the degree of the polynomial f with respect to the variable v.\n");
    fppp(fp,"Example: (x^3 + x h + 1).  (x).  degree :: ---> 3\n");
  }else if (strcmp(s,"div") == 0) {
    fppp(fp,"<< obj1  obj2 div obj3 >>\n");
    fppp(fp,"obj3 is obj1/obj2\n");
    fppp(fp,"cf. data_conversion (numerator, denominator, cancel)\n");
  }else if (strcmp(s,"dup") == 0) {
    fppp(fp,"<< obj dup obj obj >>\n");
    fppp(fp,"cf. 1 copy. Dup copies the object by the pointer.\n");
  }else if (strcmp(s,"elimination_order")==0) {
    fppp(fp,"<< (v1,...,vm) elimination_order >>\n");
    fppp(fp,"The primitive returns the weight matrix where the variables\n");
    fppp(fp,"v1, ..., vm are most expensive terms.\n");
    fppp(fp,"cf. define_ring, define_qring.\n");
  }else if (strcmp(s,"error")==0) {
    fppp(fp,"<<error>> causes kan/sm1 error.\n");
    fppp(fp,"<< s error >>   string s;\n");
    fppp(fp,"s is printed as the error message.\n");
  }else if (strcmp(s,"eval")==0) {
    fppp(fp,"<< [$key$ arguments] eval >>\n");
    fppp(fp,"key :   \n");
  }else if (strcmp(s,"exec")==0) {
    fppp(fp,"<< { executable array } exec >>\n");
    fppp(fp,"The primitive exec executes each primitive in the executable\n");
    fppp(fp,"array sequentially.\n");
    fppp(fp,"Example: {1 2 add print pop} exec ;\n");
    fppp(fp,"    ===> 3\n");
  }else if (strcmp(s,"extension")==0) {
    fppp(fp,"<< obj1 extension obj2 >>\n");
    fppp(fp,"array of object obj1; object obj2; \n");
    fppp(fp,"The first element of the obj1 should be the key word tag.\n");
    fppp(fp,"<< extension >> is used to install a new function to a system. cf.ext.c and plugin/\n");
    fppp(fp,"  [(parse) string] extension result-integer \n");
    fppp(fp,"  [(addModuleOrder) [rank_of_module pos1 weight1 pos2 weight2 ...]] extension result-object ; Execute after (mmLarger) (module_matrix) switch_function \n");
    fppp(fp,"  [(chattr) num literal] extension result-object \n");
    fppp(fp,"  [(chattrs) num ] extension result-object \n");
    fppp(fp,"  [(date)] extension date-string \n");
    fppp(fp,"  [(defaultPolyRing) num ] extension result-object \n");
    fppp(fp,"  [(flush) ] extension null \n");
    fppp(fp,"  [(getAttribute) obj key] extension value \n");
    fppp(fp,"  [(getAttributeList) obj] extension attrList \n");
    fppp(fp,"  [(getpid) ] extension result-integer \n");
    fppp(fp,"  [(getenv) envName] extension valueOfEnvName \n");
    fppp(fp,"  [(gethostname) ] extension myhostname \n");
    fppp(fp,"  [(stat) fname] extension v \n");
    fppp(fp,"       v = [null,[(error no), p]] or [0, [size ]]\n");
    fppp(fp,"  [(forkExec) argList fdList sigblock] extension pid\n");
    fppp(fp,"  [(getattr) literal] extension attr\n");
    fppp(fp,"  [(getchild)] extension listOfPid (generated by forkExec)\n");
    fppp(fp,"  [(keywords) ] extension array-of-names-of-primitives \n");
    fppp(fp,"  [(nobody) ] extension null \n");
    fppp(fp,"  [(newMatrix) m n] extension mat \n");
    fppp(fp,"  [(newVector) m ] extension vec \n");
    fppp(fp,"  [(getUniqueFileName) path] extension newName \n");
    fppp(fp,"  [(or_attr) atr literal] extension new_value \n");
    fppp(fp,"  [(or_attrs) atr ] extension result-obj \n");
    fppp(fp,"  [(outputObjectToFile) path obj] extension null \n");
    fppp(fp,"  [(ostype)] extension list \n");
    fppp(fp,"  [(quiet) flag] extension int \n");
    fppp(fp,"  [(setAttribute) obj key value] extension new-attributed-obj \n");
    fppp(fp,"  [(setAttributeList) obj attrList] extension new-attributed-obj \n");
    fppp(fp,"  [(stringToArgv) sobj] extension decomposed_str  cf. to_records \n");
    fppp(fp,"  [(stringToArgv2) sobj separator] extension decomposed_str  cf. to_records \n");
    fppp(fp,"  [(read) fd size] extension string \n");
    fppp(fp,"  [(regexec) regular_expression stringArray flags(opt)] extension list \n");
    fppp(fp,"  [(regionMatches) string stringArray] extension list \n");
    fppp(fp,"  [(traceClearStack)] extension null \n");
    fppp(fp,"  [(traceShowStack)] extension str \n");
    fppp(fp,"  [(traceShowScannerBuf)] extension str \n");
    fppp(fp,"  [(unlink) fname] extension r\n");
    /* fppp(fp,"  [(asir0) string] extension result-object \n"); */
    fppp(fp,"See also plugin-* in ?? by [(plugin)] usages :: \n");
    fppp(fp,"See also (extension-examples) usage :: \n");
  }else if (strcmp(s,"file")==0) {
    fppp(fp,"<< filename mode file fd >>\n");
    fppp(fp,"string filename, mode; file fd;\n");
    fppp(fp,"It opens the <<filename>> with the <<mode>>. <<fd>> is the\n");
    fppp(fp,"file descripter and is used in writestring and closefile.\n");
    fppp(fp,"If filename part is an integer, it calls fdopen() and returns the file descripter.\n");
    fppp(fp,"cf. closefile, writestring, pushfile \n");
  }else if (strcmp(s,"for")==0) {
    fppp(fp,"<<init inc limit {executable array} for >>\n");
    fppp(fp,"integer init inc limit;\n");
    fppp(fp,"Repeat the executable array.\n");
    fppp(fp,"Example: [ 1 1 3 {(a)} for] ::---> [1 (a) 2 (a) 3 (a)]\n");
  }else if (strcmp(s,"gbext")==0) {
    fppp(fp,"<< obj1 gbext obj2 >>\n");
    fppp(fp,"array of objects obj1;");
    fppp(fp,"The first element of the obj1 should be the key word tag.\n");
    fppp(fp,"<< gbext >> is used to call auxiliary functions for g-basis computation. \n");
    fppp(fp,"  [(divByN) poly1 n(universalNumber)] gbext [qpoly rpoly] \n");
    fppp(fp,"     where poly1 = n*qpoly+rpoly.\n");
    fppp(fp,"     (see also cancelCoeff) \n");
    fppp(fp,"  [(exponents) poly type ] gbext array \n");
    fppp(fp,"    example: type == 0    x,y,Dx,Dy\n");
    fppp(fp,"             type == 1    x,y,Dx,Dy,h,H\n");
    fppp(fp,"             type == 2    x,y,Dx,Dy,h  --- default.\n");
    fppp(fp,"  [(grade) poly1 ] gbext integer \n");
    fppp(fp,"  [(isConstant) poly] gbext bool \n");
    fppp(fp,"  [(isConstantAll) poly] gbext bool \n");
    fppp(fp,"  [(isOrdered) poly] gbext poly \n");
    fppp(fp,"  [(isReducible) poly1 poly2 ] gbext integer \n");
    fppp(fp,"  [(lcm) poly1 poly2] gbext poly \n");
    fppp(fp,"  [(lcoeff) poly] gbext poly \n");
    fppp(fp,"  [(lmonom) poly] gbext poly \n");
    fppp(fp,"  [(mod) poly1 universalNumber] gbext poly \n");
    fppp(fp,"          poly = poly1 mod universalNumber where char=0 and \n"); 
    fppp(fp,"          poly and poly2 belong to a same ring.\n");
    fppp(fp,"  [(ord_ws_all) fv wv] gbext integer \n");
    fppp(fp,"  [(reduceContent) poly] gbext [poly c] \n");
    fppp(fp,"  [(tomodp) poly1 ring] gbext poly, char(ring)>0. \n");
    fppp(fp,"          poly = poly1 mod char(ring) where poly belongs to ring.\n");
    fppp(fp,"  [(tomod0) poly1 ring] gbext poly, char(ring)=0. \n");
    fppp(fp,"  [(schreyerSkelton) array_of_poly] gbext array \n");
    fppp(fp,"  [(toes) array_of_poly] gbext poly \n");
    fppp(fp,"  [(toe_) poly] gbext poly   \n");
    fppp(fp,"  [(toe_) array_of_poly] gbext poly   cf. toVectors \n");
  }else if (strcmp(s,"get")==0) {
    fppp(fp,"<< [f0 f1 ... fn] k get fk >>\n");
    fppp(fp,"integer k;");
    fppp(fp,"The primitive gets the k-th element of a given array.\n");
    fppp(fp,"Example: [1 4 3] 1 get :: ---> 4\n");
    fppp(fp,"<< ob [k0 k1 ... ] get f >>\n");
    fppp(fp,"It is equivalent to << ob k0 get k1 get ... >>\n");
    fppp(fp,"The primitive get works for array or list.\n");
  }else if (strcmp(s,"goto")==0) {
    fppp(fp,"<< label goto >>\n");
    fppp(fp,"literal label;\n");
    fppp(fp,"Break the loops and jump to the <<literal>>.\n");
    fppp(fp,"The label <<literal>> has to be out of all the loops.\n");
    fppp(fp,"You cannot jump inside of the executable array { .... }.\n");
    fppp(fp,"Example: /point goto (skip me) message /point \n");
  }else if (strcmp(s,"groebner")==0) {
    fppp(fp,"<< [[f1 ... fn] [options]] groebner \n");
    fppp(fp,"   [[g1 ... gm] backward-transformation syzygy]>>\n");
    fppp(fp,"poly f1, ..., fn; poly g1, ..., gm;\n");
    fppp(fp,"optional return value: matrix of poly backward-transformation, syzygy;\n");  
    fppp(fp,"Computation of the Groebner basis of f1,...,fn. The basis is {g1,...,gm}.\n");
    fppp(fp,"Options: << (needBack), (needSyz), (reduceOnly), (gbCheck),\n");
    fppp(fp,"            (countDown) number (StopDegree) number, (forceReduction)>>\n");
    fppp(fp,"Flags:<< [(ReduceLowerTerms) 1] system_variable >>\n");
    fppp(fp,"      << [(AutoReduce) 0]          system_variable >>\n");
    fppp(fp,"      << [(UseCriterion1) 0]    system_variable >>\n");
    fppp(fp,"      << [(UseCriterion2B) 0]   system_variable >>\n");
    fppp(fp,"      << [(Sugar) 0]            system_variable >>\n");
    fppp(fp,"      << [(Homogenize) 1]       system_variable >>\n");
    fppp(fp,"      << [(CheckHomogenization) 1] system_variable >>\n");
    fppp(fp,"      << [(Statistics) 1]       system_variable >>\n");
    fppp(fp,"      << [(KanGBmessage) 1]     system_variable >>\n");
    fppp(fp,"      << [(Verbose) 0]          system_variable >>\n");
    fppp(fp,"Example: [(x0,x1) ring_of_polynomials 0] define_ring\n");
    fppp(fp,"         [(x0^2+x1^2-h^2). (x0 x1 -4 h^2).] /ff set ;\n");
    fppp(fp,"         [ff] groebner /gg set ;\n");
    fppp(fp,"         gg ::\n");
    fppp(fp,"cf. homogenize, groebner_sugar, define_ring, \n");
    fppp(fp,"    ring_of_polynomials, ring_of_differential_operators.\n");
  }else if (strcmp(s,"homogenize") == 0) {
    fppp(fp,"<< f homogenize g >>\n");
    fppp(fp,"poly f,g;\n");
    fppp(fp,"array of poly f,g;\n\n");
	fppp(fp,"[(degreeShift) (value)] homogenize [shiftD shiftUV]\n");
	fppp(fp,"[(degreeShift) (reset)] homogenize [null null]\n");
	fppp(fp,"[(degreeShift) shiftD ] homogenize [shiftD shiftUV]\n");
	fppp(fp,"[(degreeShift) [shiftD shiftUV]] homogenize [shiftD shiftUV]\n");
	fppp(fp,"[(degreeShift) [ ]     fv] homogenize   hfv\n");
	fppp(fp,"[(degreeShift) shiftD  fv] homogenize   hfv\n");
	fppp(fp,"[(degreeShift) [shiftD shiftUV] fv] homogenize hfv\n");
    fppp(fp,"shiftD : degree shift vector for (0,1)-h homogenization\n");
    fppp(fp,"shiftUV : degree shift vector for (-1,1)-s homogenization (internal for ecart.)\n");
    fppp(fp,"fv : polynomial or vector of polynomials.\n");
    fppp(fp,"Example:  [(x) ring_of_differential_operators 0] define_ring\n");
	fppp(fp,"   [(degreeShift) [[1 0] [0 1]] [(x+1). (Dx+1).]] homogenize ::\n");
	fppp(fp,"   [(degreeShift) (value)] homogenize ::\n");
	fppp(fp,"Note. min of ord (-1,1)[0,1] is min {-1,1, 1+1,1+0} = -1 =m \n");
    fppp(fp,"      Degree of H is b-a+v(i)-m where v=[0,1]\n");
    fppp(fp,"Side effects:  It changes h-degree shift vector and s-degree shift vector\n");
	fppp(fp,"   in homogenizeObject_go(), which is called from ecart division codes\n");
    fppp(fp,"   as well as the function homogenize.\n");
    fppp(fp,"DegreeShift is automatically reset when set_up_ring is called.\n");
  }else if (strcmp(s,"idiv") == 0) {
    fppp(fp,"<< a b idiv a/b >>\n");
    fppp(fp,"int a,b,a/b;\n");
    fppp(fp,"Example:  5 2 idiv ::  ===> 2\n");
  }else if (strcmp(s,"ifelse") == 0) {
    fppp(fp,"<< condition { true case } { false case } ifelse >>\n"); /*:*/
    /* Tenuki mark --------------^ */
    fppp(fp,"integer condition;\n");
    fppp(fp,"If condition is non-zero, then true-case will be executed,\n");
    fppp(fp,"else false-case will be executed.\n");
  }else if (strcmp(s,"index") == 0) {
    fppp(fp,"<<  a_0 ... a_{n-1} n index a_0 ... a_{n-1} a_0 >>\n");/*:*/
    fppp(fp,"integer n; \n");
    fppp(fp,"It takes a_0 from the stack.\n");
  }else if (strcmp(s,"init")==0) {
    fppp(fp,"<< f init h >>\n");
    fppp(fp,"<< f weight_vector init h >>\n");
    fppp(fp,"poly f,h; array of integer weight_vector;\n");
    fppp(fp,"h is the initial term of the polynomial f.\n");
    fppp(fp,"Example: (2 x^3 + 3 x + 1). init :: ---> 2 x^3\n");
    fppp(fp,"Example: [(x^3+1). (x h+ h^5).] {init} map ::\n");
    fppp(fp,"Example: (x+1). [0 1 1 1] init ::    cf. weightv \n\n");
    fppp(fp,"<< fv weight_vector init h >>\n");
    fppp(fp,"<< fv [weight_vector shift_vector] init h >>\n");
    fppp(fp,"<< fv init h >> or << hv [ ] init h >>\n");
    fppp(fp,"fv is a polynomial or a vector of polynomials.\n");
    fppp(fp,"h is the initial term with respect to the weight vector and the shift vector\n");
    fppp(fp,"if they are given.\n");
    fppp(fp,"Note: the last x variable is always assumed to be the vector index.\n");
    fppp(fp,"Example: [(x,y) ring_of_differential_operators 0] define_ring\n");
	fppp(fp,"    [(x^2*Dx^2+2*x*Dx+x). (Dx+x).] [[(Dx) 1 (x) -1] weightv [0 -1]] init ::\n");
  }else if (strcmp(s,"lc")==0) {
    fppp(fp,"<< obj lc leftCell >>\n");
    fppp(fp,"class leftCell;\n");
    fppp(fp,"leftCell is the left cell of the obj of which type must be class.\n");
  }else if (strcmp(s,"length")==0) {
    fppp(fp,"<< [a0 a1 ... an] length <n+1> >>\n");
    fppp(fp,"<< s length p >>\n");
    fppp(fp,"integer <n+1>; integer p; string s;\n");
    fppp(fp,"It returns the length of the argument. p is the length of the string.\n");
  }else if (strcmp(s,"load")==0) {
    fppp(fp,"<< /literal load obj >>\n");
    fppp(fp,"<<load>> gets the value of <<literal>>.\n");
  }else if (strcmp(s,"loop") == 0) {
    fppp(fp,"<< {executable array} loop >>\n"); /*:*/
    fppp(fp,"cf. exit\n");
  }else if (strcmp(s,"map")==0) {
    fppp(fp,"<< [f1 ... fn] {code} map [f1 {code}exec ... fn {code}exec] >>\n");
    fppp(fp,"map applies {code} to each element in [f1 ... fn].\n");
    fppp(fp,"Example: [1 2 3] {1 add} map ===> [2 3 4]\n");
  }else if (strcmp(s,"mpzext") == 0) {
    fppp(fp,"[(key) arguments] mpzext ans\n"); /*:*/
    fppp(fp,"It calls mpz functions.\n");
    fppp(fp,"arguments should be numbers(universalNumber).\n");
    fppp(fp,"Keys: gcd, tdiv_qr, cancel, powm, probab_prim_p, sqrt, and, ior, com\n");
    fppp(fp,"For details, see gmp-2.0.2/texinfo.tex.\n");
  }else if (strcmp(s,"mul") ==0) {
    fppp(fp,"obj1 obj2 mul obj3\n");
    fppp(fp,"obj3 = obj1 * obj2\n");
  }else if (strcmp(s,"newcontext") == 0) {
    fppp(fp,"<< name super newcontext class.Context >>\n"); /*:*/
    fppp(fp,"It creates new context and new user dictionary.\n"); /*:*/
    fppp(fp,"Ex. (mycontext) StandardContextp newcontext /nc set.\n");
    fppp(fp,"cf. setcontext.\n");
  }else if (strcmp(s,"newstack") == 0) {
    fppp(fp,"<< size newstack class.OperandStack >>\n"); /*:*/
    fppp(fp,"cf. setstack, stdstack.\n");
  }else if (strcmp(s,"oxshell") == 0) {
    fppp(fp,"<< cmds oxshell result >>\n"); /*:*/
    fppp(fp,"cmds is an array of strings.\n");
	KoxShellHelp((char *)NULL,fp);
  }else if (strcmp(s,"pop")==0) {
    fppp(fp,"<< obj pop >>  \n"); /*:*/
    fppp(fp,"It removes the obj from the stack.\n");
  }else if (strcmp(s,"principal")==0) {
    fppp(fp,"<< obj principal obj >>  Get the principal part.\n"); /*:*/
    fppp(fp,"This is an old command. init should be used.\n");
    fppp(fp,"Example: (x Dx + Dy^2+ y Dx Dy). [(Dx) 1 (Dy) 1] weightv init\n");
  }else if (strcmp(s,"print")==0) {
    fppp(fp,"<< obj print >> \n"); /*:*/
    fppp(fp,"Print the object to stdout. cf. obj (string) dc \n");
  }else if (strcmp(s,"print_switch_status")==0) {
    fppp(fp,"<< print_switch_status >>\n"); /*:*/
    fppp(fp,"cf. switch_function\n");
  }else if (strcmp(s,"primmsg")==0) {
    fppp(fp,"<< { executable array } primmsg >> \n");
    fppp(fp,"The << executable array >> is executed in the PrimitiveContext.\n");
    fppp(fp,"cf. sendmsg, sendmsg2, supmsg2, system_variable\n");
  }else if (strcmp(s,"pstack")==0) {
    fppp(fp,"<< pstack >>\n");
    fppp(fp,"The operator prints the entire contents of the stack.\n");
  }else if (strcmp(s,"pushfile")==0) {
    fppp(fp,"<< filename pushfile sss >>\n");
    fppp(fp,"string filename, sss ; \n");
    fppp(fp,"Read the file and push the contents of file as the string.\n");
    fppp(fp,"The file will be search in the search path as run operator.\n");
    fppp(fp,"Example: (myfile.txt) pushfile /sss set \n");
    fppp(fp,"cf:  [(parse) sss] extension\n");
  }else if (strcmp(s,"put")==0) {
    fppp(fp,"<< [a0 a1 ... ap] i any put >>\n");
    fppp(fp,"<< [a0 a1 ... ap] multi-index any put >>\n");
    fppp(fp,"<< s i any put >>\n");
    fppp(fp,"integer i; string s; array of integers multi-index;\n");
    fppp(fp,"The operator put <<any>> at the place i.\n");
    fppp(fp,"Example: /a [1 2 3] def  a 2 (Hi) put a :: --->[1 2 (Hi)]\n");
    fppp(fp,"Example: /a [[1 2] [3 4]] def  a [0 1] 10 put a :: --->\n");
    fppp(fp,"                                               [[1 10] [3 4]]\n");
  }else if (strcmp(s,"rc")==0) {
    fppp(fp,"<< obj rc rightCell >>\n");
    fppp(fp,"rightCell is the right cell of the obj of which type must be class.\n");
    fppp(fp,"class rightCell;\n");
  }else if (strcmp(s,"reduction")==0) {
    fppp(fp,"<< f [g_1 g_2 ... g_m] reduction [h c0 syz input] >>\n");
    fppp(fp,"poly f, g1_1, ..., g_m, h, c0; array of poly syz, input;\n");
    fppp(fp,"h is the normal of f by {g1 .... fm}.\n");
    fppp(fp,"h = c0 f + \\sum syz_i g_i \n");
  }else if (strcmp(s,"replace")==0) {
    fppp(fp,"<< f [[L1 R1] ... [Ln Rn]] replace g >>\n");
    fppp(fp,"poly f, L1, R1, ..., Ln, Rn, g; \n");
    fppp(fp,"<<replace>> applies the rules L1-->R1, ..., Ln-->Rn to transform f.\n");
    fppp(fp,"Example: (x^2+x+1). [[(x). (2).]] replace :: ---> 7\n");
  }else if (strcmp(s,"roll")==0) {
    fppp(fp,"<< f(n-1) ... f(0) n j roll f(j-1 mod n) f(j-2 mod n) ... f(j-n mod n) >>\n");
    fppp(fp,"integer n, j;\n");
    fppp(fp,"Example: [(a) (b) (c) 3 1 roll] ::  ==>[(c) (a) (b)]\n");
  }else if (strcmp(s,"run") == 0) {
    fppp(fp,"<< filename run >>\n"); /*:*/
    fppp(fp,"string filename ;\n"); /*:*/
    fppp(fp,"Example. (restriction.sm1) run \n");
    fppp(fp,"File will be searched in the current directory, /usr/local/lib/sm1,\n");
    fppp(fp,"and the directory specified by the environment variable LOAD_SM1_PATH\n");
    fppp(fp,"Example. setenv LOAD_SM1_PATH ~/lib/sm1/ \n");
  }else if (strcmp(s,"sendmsg") == 0) {
    fppp(fp,"<< obj { executable array } sendmsg >>\n"); /*:*/
    fppp(fp,"When obj is in k-class (i.e. obj is of the form \n");
    fppp(fp," [<<class.Context>> , body ]), then the CurrentContextp is changed\n");
    fppp(fp,"to <<class.Context>> and the obj <<executable array>> will be executed.\n");
    fppp(fp,"If not, CurrentContextp is changed to the PrimitiveContextp and\n");
    fppp(fp,"the obj <<executable array>> will be executed.\n");
    fppp(fp,"cf. setcontext, newcontext, StandardContextp, system_variable,\n");
    fppp(fp,"    sendmsg2, primmsg, supmsg2.\n");
  }else if (strcmp(s,"sendmsg2") == 0) {
    fppp(fp,"<< obj2 obj3 { executable array } sendmsg2 >>\n"); /*:*/
    fppp(fp,"Context is determined by obj2 if obj2 is class.context.\n");
    fppp(fp,"If not, it is determined by obj3 if obj2 is class.context.\n");
    fppp(fp,"cf. sendmsg.\n");
  }else if (strcmp(s,"set_order_by_matrix")==0) {
    fppp(fp,"<< order-matrix set_order_by_matrix >>\n"); /*:*/
    fppp(fp,"matrix of integers order-matrix; \n");
    fppp(fp,"cf. weight_vector, show_ring, elimination_order, switch_function\n");
    fppp(fp,"    system_variable \n");
  }else if (strcmp(s,"set_timer")==0) {
    fppp(fp,"cf. timer\n"); /*:*/
  }else if (strcmp(s,"set_up_ring@")==0) {
    fppp(fp,"<< [x-variables] [d-variables] [p c l m n cc ll mm nn next] order-matrix \n");
    fppp(fp,"   [(keyword) value (keyword) value ....]  set_up_ring@ >>\n");
    fppp(fp,"<<next>> is the optional argument. The last argument is also optional.\n");
    fppp(fp,"Keywords are mpMult, coeffient ring, valuation, characteristic,\n");
    fppp(fp,"             schreyer, ringName.\n\n");
    fppp(fp,"1.When [x[0] .... x[n-1]] [D[0] .... D[n-1]] is given as the lists\n");
    fppp(fp,"  of variables, D[0] is usually used as the variable for homogenization\n");
    fppp(fp,"  and x[n-1] is used for the variable for the graduation.\n");
    fppp(fp,"2.Order matrix should be given in the order x[n-1] ... x[0], D[n-1]...D[0]\n");
    fppp(fp,"3.0<=i<c : commutative; c<=i<l : q-difference;\n");
    fppp(fp,"  l<=i<m : difference(better not to use it); m<=i<n : differential;\n");
    fppp(fp,"4.Graduation variables :\n");
    fppp(fp,"  cc<=i<c : commutative; ll<=i<l : q-difference;\n");
    fppp(fp,"  mm<=i<m : difference;  nn<=i<n : differential;\n");
    fppp(fp,"  If you do not use graduation variables, set, for example, cc=c.\n");
    fppp(fp,"5.c-cc>0 or l-ll>0 or m-mm>0 or n-nn>0 must be held.\n\n");
    fppp(fp,"Example: [$H$ $x$ $y$ $e$] [$h$ $Dx$ $Dy$ $E$]\n");
    fppp(fp,"         [0 1 1 1 4 1 1 1 3]\n");
    fppp(fp,"        (  e y x H   E Dy Dx h )\n");
    fppp(fp,"         [[1 1 1 1   1 1  1  0]\n");
    fppp(fp,"          [1 0 0 0   0 0  0  0]\n");
    fppp(fp,"          [0 0 0 0   0 1  0  0]\n");
    fppp(fp,"           ........\n\n");
    fppp(fp,"cf. polynomial_ring, ring_of_..., groebner.\n");
  }else if (strcmp(s,"setcontext")==0) {
    fppp(fp,"<< class.Context setcontext >>\n");
    fppp(fp,"Change the CurrentContextp (and the user dictionary) ");
    fppp(fp,"to class.Context.\n");
    fppp(fp,"Ex. (mynew) StandardContextp newcontext /cp set cp setcontext\n");
    fppp(fp,"cf. system_variable(CurrentContextp), newcontext,\n");
    fppp(fp,"    show_user_dictionary, StandardContextp.\n");
  }else if (strcmp(s,"setstack")==0) {
    fppp(fp,"<< class.OperandStack setstack >>\n");
    fppp(fp,"Change the current operand stack to class.OperandStack.\n");
    fppp(fp,"Ex. 1000 newstack /st set  st setstack\n");
    fppp(fp,"cf. newstack, stdstack\n");
  }else if (strcmp(s,"show_ring")==0 ) {
    fppp(fp,"<< show_ring >>\n"); /*:*/
    fppp(fp,"cf. set_up_ring.\n");
  }else if (strcmp(s,"show_systemdictionary") == 0 || strcmp(s,"show_user_dictionary")==0 ) {
    fppp(fp,"<< show_systemdictionary >>  << show_user_dictionary >>\n"); /*:*/
  }else if (strcmp(s,"spol")==0) {
    fppp(fp,"<< f  g spol  [ [c d] r] >>\n"); /*:*/
    fppp(fp,"poly f, g, c, d, r;\n");
    fppp(fp,"r is the s-polynomial of f and g.\n");
    fppp(fp," r = c f + d g\n");
  }else if (strcmp(s,"stdstack")==0) {
    fppp(fp,"<< stdstack >>\n");
    fppp(fp,"Change the current operand stack to the standard stack.\n");
    fppp(fp,"cf. newstack, setstack\n");
  }else if (strcmp(s,"sub") ==0) {
    fppp(fp,"obj1 obj2 sub obj3\n");
    fppp(fp,"obj3 = obj1 - obj2.\n");
  }else if (strcmp(s,"supercontext")==0) {
    fppp(fp,"<< class.Context supercontext class.Context2 >>\n"); /*:*/
    fppp(fp,"cf. setcontext, newcontext\n");
  }else if (strcmp(s,"supmsg2") == 0) {
    fppp(fp,"<< obj2 obj3 { executable array } supmsg2 >>\n"); /*:*/
    fppp(fp,"Context is the super of obj2 or obj3.\n");
    fppp(fp,"cf. sendmsg, sendmsg2, primmsg.\n");
  }else if (strcmp(s,"switch_function")==0) {
    fppp(fp,"<< (function) (name) switch_function >>\n"); /*:*/
    fppp(fp,"<< (report) (function) switch_function value >>\n"); /*:*/
    fppp(fp,"cf. print_switch_status, getOptions, restoreOptions.\n");
  }else if (strcmp(s,"system")==0) {
    fppp(fp,"<< command system >>\n"); /*:*/
    fppp(fp,"string command; \n");
    fppp(fp,"Execute unix command.\n");
    fppp(fp,"Example: (ls) system \n");
  }else if (strcmp(s,"system_variable")==0) {
    fppp(fp,"<< [(key word)] system_variable result >> or\n");
    fppp(fp,"<< [(key word) value] system_variable result >>\n");
    fppp(fp,"<< [(key word) optional arg] system_variable result >>\n");
    fppp(fp,"The primitive returns the value of a given system constant.\n");
    fppp(fp,"The primitive sets the value of a given system constant.\n");
    fppp(fp,"PrintDollar [0,1,2], Wrap, P, N, NN, M, MM, L, LL, C, CC,ringName,\n");
    fppp(fp,"CurrentRingp, Verbose, UseCriterion1, UseCriterion2B,\n");
    fppp(fp,"ReduceLowerTerms, CheckHomogenization, Homogenize, Sugar, Homogenize_vec,\n");
    fppp(fp,"Statistics, StackPointer, StandardOperandStack,\n");
    fppp(fp,"ErrorStack, ErrorMessageMode, WarningMessageMode,\n");
    fppp(fp,"CatchCtrlC, Strict, CurrentContextp, NullContextp, Strict2, QuoteMode\n");
    fppp(fp,"SigIgn, KSPushEnvMode, PrimitiveContextp, KanGBmessage,\n");
    fppp(fp,"orderMatrix, gbListTower, Schreyer, \n");
    fppp(fp,"outputOrder, multSymbol, variableNames, Version,\n");
    fppp(fp,"AvoidTheSameRing, RingStack, LeftBracket, RightBracket,\n");
    fppp(fp,"AutoReduce, Ecart, EcartAutomaticHomogenization\n");
    fppp(fp,"DoCancel, DebugContentReduction\n");
    fppp(fp,"RestrictedMode (cf. or_attr 8), SecureMode(obsolete) \n");
    fppp(fp,"UseDsmall, COutput \n");
    fppp(fp,"Example: [(N)] system_variable\n");
    fppp(fp,"                             (maximum number of variables)/2.\n");
    fppp(fp,"         [(x) (var) 3] system_variable\n");
    fppp(fp,"                              The 3rd variable.\n");
    fppp(fp,"         [(D) (var) 4] system_variable\n");
    fppp(fp,"                              The 4th differential operator.\n");
    fppp(fp,"         [(PrintDollar) 0] system_variable\n");
    fppp(fp,"                          Set the global variable PrintDollar to 0\n");
//    fppp(fp,"DebugReductionEcart (bit):  (2) *% (1) find_reducer,in(r) \n");
    fppp(fp,"DebugReductionEcart (bit):  (2) * (1) find_reducer,in(r) \n");
    fppp(fp,"              (any) cont   (4) #+[rat division, + ecartd] \n");
    fppp(fp,"                           (8) check the order. \n");
    fppp(fp,"DoCancel (bit):  (2) always_reduce_content  (1) Noro_strategy \n");
    fppp(fp,"                 (4) DoCancel_in_reduction1 \n");
  }else if (strcmp(s,"tlimit")==0) {
    fppp(fp,"<< { ... } t tlimit results >>\n");
    fppp(fp,"Limit the execution time to t seconds.\n");
    fppp(fp,"When t is not positive, tlimit pushes the execution time.\n");
  }else if (strcmp(s,"to_records")==0) {
    fppp(fp,"<< ({arg1,arg2,...,argn})  to_records (arg1) ... (argn) n >>\n");
    fppp(fp,"Example: ({x,y2}) to_records ===> (x) (y2) 2\n");
    fppp(fp,"Example: (x,y2)   to_records ===> (x) (y2) 2\n");
    fppp(fp,"Example: [(x,y2) to_records pop] ===> [(x) (y2)]\n");
    fppp(fp,"cf. [(stringToArgv) str] extension or stringToArgv2\n");
  }else if (strcmp(s,"writestring") == 0) {
    fppp(fp,"<< fd s writestring >>\n"); /*:*/
    fppp(fp,"file fd; string s;\n");
    fppp(fp,"Write the string s to the file fd.\n");
    fppp(fp,"cf. [(PrintDollar)] system_variable, file, closefile\n");
    /******* macros ************/
  }else if (strcmp(s,"timer")==0) {
    fppp(fp,"<< { executable array } timer >>\n"); /*:*/
  }else{
    fppp(fp,"The keyword <<%s>> cannot be found.\n",s);
    fppp(fp,"Type in ? in order to see system dictionary.\n");
  }    
}
