/**
 * $OpenXM: OpenXM/src/OpenMath/OM2OXM.java,v 1.11 1999/11/17 08:37:29 tam Exp $
 *
 * このクラスでは以下の BNF で表される構文解析を実装している
 * expr -> stag [expr | immediate]* etag
 * immediate -> INTEGER
 *            | SYMBOL
 * stag -> '<' SYMBOL '>'
 * etag -> '<' '/' SYMBOL '>'
 * 終端記号: INTEGER, SYMBOL, '<', '/', '>'
 * 非終端記号: expr, stag, etag, immediate
 * 開始記号: expr
 *
 * 以下の関数を使用:
 * public void send(OutputStream os); - buffer にある OX message を送信する.
 * public int parse(); - 構文解析を開始する.
 * private int parse_expr();       - parser の expr 部分
 * private Attribute parse_stag(); - parser の stag 部分
 * private void parse_etag(Attribute tag,int argnum); - parser の etag 部分
 * private int parse_immediate();  - parser の immediate 部分
 * private void parse_error(String mesg); - parser の error を表示して終了する
 * public static void main(String[] args); - main 関数.
 *
 * 以下の変数を使用:
 * Attribute Attribute; - 字句解析器を呼び出し, その属性値を保存しておく
 * int token; - 現在読み込んでいる token を保存しておくのに使用.
 * CellStack stack; - セマンティックスのチェックに使う内部スタック.
 * ByteArrayOutputStream buffer; - oxasir に送信するまえにデータを溜めておく
 */

import java.io.*;
import java.net.*;
import java.util.Vector;
import JP.ac.kobe_u.math.tam.OpenXM.*;

final class OM2OXM implements Runnable{
  private PushbackInputStream is;
  private String attribute;
  private int token = TT_NULL;
  private boolean lexPushbackFlag = false;
  private OpenXM asir; // for debug
  private boolean debug = false;

  // Token Type for lexical analyzer
  final static int TT_NULL      = 0;
  final static int TT_String    = -1;
  final static int TT_StartTag  = -2;
  final static int TT_EndTag    = -3;
  final static int TT_SingleTag = -4;

  public void run(){ // for debug
    //サーバ側から送信された文字列を受信します。
    try{
      while(true){
	CMO tmp;

	switch(asir.receiveOXtag()){
	case OpenXM.OX_COMMAND:
          asir.receiveSM();
          break;

	case OpenXM.OX_DATA:
          tmp = asir.receiveCMO();
	  System.out.println("=> "+ CMO2OM(tmp));
          break;
	}

      }
    }catch(IOException e){}
  }

  public static String CMO2OM(CMO cmo){
    return "<OMOBJ>"+ CMO2OM_sub(cmo) +"</OMOBJ>";
  }

  private static String CMO2OM_sub(CMO cmo) throws NumberFormatException{
    String ret = "";

    switch(cmo.getDISCRIMINATOR()){
    case CMO.CMO_NULL:
      return "<OMI>0</OMI>";

    case CMO.CMO_INT32:
      return "<OMI>"+ ((CMO_INT32)cmo).intValue() +"</OMI>";

      // case CMO.CMO_DATUM:

    case CMO.CMO_STRING:
      return "<OMSTR>"+ ((CMO_STRING)cmo).getString() +"</OMSTR>";

      // case CMO.CMO_LIST:

    case CMO.CMO_MONOMIAL32:
      ret += "<OMA><OMS name=\"Monom\" cd=\"poly\"/>";
      ret += CMO2OM_sub(((CMO_MONOMIAL32)cmo).getCoefficient());
      for(int i=0;i<((CMO_MONOMIAL32)cmo).getDegree().length;i++){
	ret += "<OMI>"+ ((CMO_MONOMIAL32)cmo).getDegree()[i] +"</OMI>";
      }
      ret += "</OMA>";
      return ret;

    case CMO.CMO_ZZ:
      return "<OMI>"+ ((CMO_ZZ)cmo).BigIntValue() +"</OMI>";

    case CMO.CMO_QQ:
      return "<OMA><OMS name=\"over\" cd=\"basic\"/>"+
	CMO2OM_sub(((CMO_QQ)cmo).getBunshi())+
	CMO2OM_sub(((CMO_QQ)cmo).getBunbo())+
	"</OMA>";

    case CMO.CMO_ZERO:
      return "<OMI> 0 </OMI>";

      // case CMO.CMO_DMS:

    case CMO.CMO_DMS_GENERIC:
      ret += "<OMA><OMS name=\"PolyRing\" cd=\"poly\"/>";
      ret += "<OMI>2</OMI></OMA>";
      return ret;

    case CMO.CMO_RECURSIVE_POLYNOMIAL:
      return CMO2OM_CoefficientOfRecursivePOLYNOMIAL(((CMO_RECURSIVE_POLYNOMIAL)cmo).getPolynomial(),((CMO_RECURSIVE_POLYNOMIAL)cmo).getVariables());

    case CMO.CMO_DISTRIBUTED_POLYNOMIAL:
      ret += "<OMA><OMS name=\"DMP\" cd=\"poly\"/>";
      ret += CMO2OM_sub(((CMO_DISTRIBUTED_POLYNOMIAL)cmo).getRing());
      ret += "<OMA><OMS name=\"SDMP\" cd=\"poly\"/>";
      for(int i=0;i<((CMO_DISTRIBUTED_POLYNOMIAL)cmo).getMonomials().length;
	  i++){
	ret += CMO2OM_sub(((CMO_DISTRIBUTED_POLYNOMIAL)cmo).getMonomials()[i]);
      }
      ret += "</OMA></OMA>";
      return ret;

      //case CMO.CMO_POLYNOMIAL_IN_ONE_VARIABLE:

    case CMO.CMO_BIGFLOAT:
      ret += "<OMA><OMS name=\"times\" cd=\"basic\"/>";
      ret += CMO2OM_sub(((CMO_BIGFLOAT)cmo).getSyosubu());
      ret += "<OMA><OMS name=\"power\" cd=\"basic\"/>";
      ret += "<OMI>2</OMI>";
      ret += CMO2OM_sub(((CMO_BIGFLOAT)cmo).getShisubu());
      ret += "</OMA></OMA>";
      return ret;

    case CMO.CMO_INDETERMINATE:
      return "<OMV name=\""+ ((CMO_INDETERMINATE)cmo).getString() +"\"/>";

    case CMO.CMO_TREE:
      ret += "<OMA><OMS name=\""+ ((CMO_TREE)cmo).getName() +"\" cd=\""+
	((CMO_TREE)cmo).getCDName() +"\"/>";
      for(int i=0;i<((CMO_TREE)cmo).getLeaves().getElement().length;i++){
	ret += CMO2OM_sub(((CMO_TREE)cmo).getLeaves().getElement()[i]);
      }
      ret += "</OMA>";
      return ret;

    default:
      //return "<OMSTR>"+ cmo.toCMOexpression() +"</OMSTR>";
    }

    throw new NumberFormatException("unknown convert way:"+
				    cmo.toCMOexpression());
  }

  private static String CMO2OM_CoefficientOfRecursivePOLYNOMIAL(CMO cmo,CMO_LIST variables){
    CMO_POLYNOMIAL_IN_ONE_VARIABLE poly;
    String ret = "",variable;

    if(!(cmo instanceof CMO_POLYNOMIAL_IN_ONE_VARIABLE)){
      return CMO2OM_sub(cmo);
    }

    poly = (CMO_POLYNOMIAL_IN_ONE_VARIABLE)cmo;
    variable = CMO2OM_sub(variables.getElement()[poly.getVariable()]);
 
   for(int i=0;i<poly.getDegrees().length;i++){
      String mono;

      mono = "<OMA><OMS name=\"times\" cd=\"basic\"/>"+
	"<OMA><OMS name=\"power\" cd=\"basic\"/>"+
	variable +"<OMI>"+ poly.getDegrees()[i] +"</OMI></OMA>"+
	CMO2OM_CoefficientOfRecursivePOLYNOMIAL(poly.getCoefficients()[i],
						variables)
	+"</OMA>";
      if(i==0){
	ret = mono;
      }else{
	ret = "<OMA><OMS name=\"plus\" cd=\"basic\"/>"+ ret + mono +"</OMA>";
      }
    }

   return ret;
  }

  private boolean isSpace(int ch){ // use from lex
    return (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r');
  }

  private int skipSpace() throws IOException{ // use from lex
    int ch;

    do{
      ch = is.read();
    }while(isSpace(ch));
    is.unread(ch);

    return ch;
  }

  private void pushbackLex(){
    if(lexPushbackFlag){
      System.err.println("lex pushback error!");
    }
    lexPushbackFlag = true;
  }

  private int readNextToken() throws IOException{ // lexical analyzer
    String buf = "";
    int ch;

    if(lexPushbackFlag){
      lexPushbackFlag = false;
      return token;
    }

    token = TT_NULL;
    attribute = "";
    skipSpace();
    ch = is.read();

    if(ch == '<'){ // for tag
      ch = skipSpace();
      if(ch == '/'){
	is.read();
	token = TT_EndTag;
	skipSpace();
      }else{
	token = TT_StartTag;
      }
      while((ch = is.read()) != '>' && ch != '/' && ch != -1){
	if(isSpace(ch)){
	  if((ch = skipSpace()) != '>' && ch != '/'){
	    buf += " ";
	  }
	  continue;
	}

	buf += Character.toUpperCase((char)ch);

	if(ch == '"'){
	  do{
	    ch = is.read();
	    buf += (char)ch;
	  }while(ch != '"' && ch != -1);
	}
      }
      if(ch == '>'){ // for StartTag and EndTag
	attribute = buf;
	return token;
      }else if(ch == '/'){
	skipSpace();
	if((ch = is.read()) == '>'){ // for SingleTag
	  attribute = buf;
	  return token = TT_SingleTag;
	}
      }
      return token = TT_NULL;
    }else if(ch != -1){
      is.unread(ch);
      while((ch = is.read()) != '<' && ch != -1){
	debug("debug: "+ch);
	if(isSpace(ch)){
	  String spaces = String.valueOf((char)ch);

	  while(isSpace(ch = is.read())){
	    spaces += (char)ch;
	  }
	  if(ch != '<' && ch != -1){
	    buf += spaces;
	  }
	  is.unread(ch);
	}else{
	  buf += (char)ch;
	}
      }
      is.unread(ch);

      attribute = buf;
      return token = TT_String;
    }

    return token = TT_NULL;
  }

  private boolean exceptTokenTypeInParse(int type) throws IOException{
    // use from parse
    if(readNextToken() != type){
      parse_error("We expect type :'"+ type
		  +"', but we got type :'"+ token +"'("+ attribute +").");
    }
    //debug(":"+token+":"+attribute+":"+type);
    return true;
  }

  public CMO parse(InputStream stream) throws IOException{
    // start -> '<OMOBJ>' object '</OMOBJ>'
    CMO ret;

    is = new PushbackInputStream(stream);

    exceptTokenTypeInParse(TT_StartTag);
    if(!attribute.equals("OMOBJ")){
      parse_error("We expect '<OMOBJ>'.");
    }

    ret = parse_object();

    exceptTokenTypeInParse(TT_EndTag);
    if(!attribute.equals("OMOBJ")){
      parse_error("We expect '</OMOBJ>'.");
    }

    return ret;
  }

  private CMO parse_object() throws IOException{
    // object -> variable
    //         | '<OMI>' S? integer S? '</OMI>'
    //         | '<OMSTR>' S? utf7 S? '</OMSTR>'
    //         | '<OMA>' S? symbol S? objects S? '</OMA>'
    CMO ret;

    if(readNextToken() != TT_StartTag && token != TT_SingleTag){
      parse_error("We expect '<OMI> or '<OMA>' or '<OMV>'.");
    }

    if(attribute.startsWith("OMV")){
      pushbackLex();
      ret = parse_variable();
    }else if(attribute.equals("OMI")){
      pushbackLex();
      ret = parse_OMI();
    }else if(attribute.equals("OMSTR")){
      pushbackLex();
      ret = parse_OMSTR();
    }else if(attribute.equals("OMA")){
      String name,cdname;
      int argnum = 0;

      name = parse_symbol();

      if(name.equals("DMP")){
	ret = parse_symb_DMP();
	debug("poly: "+ret);
      }else{
	ret = new CMO_TREE(name,"Basic",parse_objects());
      }

      exceptTokenTypeInParse(TT_EndTag);
      if(!attribute.equals("OMA")){
	parse_error("We expect '</OMA>'.");
      }
    }else{
      parse_error("???");
      ret = null;
    }

    return ret;
  }

  private CMO_ZZ parse_OMI() throws IOException{
    CMO_ZZ ret;

    exceptTokenTypeInParse(TT_StartTag);
    if(!attribute.equals("OMI")){
      parse_error("We expect '<OMI>'.");
    }

    ret = (CMO_ZZ)parse_integer();

    exceptTokenTypeInParse(TT_EndTag);
    if(!attribute.equals("OMI")){
      parse_error("We expect '</OMI>'.");
    }

    return ret;
  }

  private CMO_STRING parse_OMSTR() throws IOException{
    CMO_STRING ret;

    exceptTokenTypeInParse(TT_StartTag);
    if(!attribute.equals("OMSTR")){
      parse_error("We expect '<OMSTR>'.");
    }

    if(readNextToken() == TT_String){
      //ret = (CMO_STRING)parse_utf7();
      ret = new CMO_STRING(attribute);
    }else{
      ret = new CMO_STRING("");
      pushbackLex();
    }

    exceptTokenTypeInParse(TT_EndTag);
    if(!attribute.equals("OMSTR")){
      parse_error("We expect '</OMSTR>'.");
    }

    return ret;
  }

  private CMO parse_symb_DMP() throws IOException{
    parse_object();
    return new CMO_DISTRIBUTED_POLYNOMIAL(new CMO_DMS_GENERIC(),
					  parse_symb_SDMP());
  }

  private CMO_MONOMIAL32[] parse_symb_SDMP() throws IOException{
    Vector mono = new Vector();
    CMO_MONOMIAL32[] ret;

    exceptTokenTypeInParse(TT_StartTag);
    if(!attribute.equals("OMA")){
      parse_error("We expect '<OMA>'.");
    }

    if(!parse_symbol().equals("SDMP")){
      parse_error("We expect '<SDMP>'");
    }

    while(readNextToken() != TT_EndTag){
      pushbackLex();
      mono.addElement(parse_symb_Monom());
    }

    if(!attribute.equals("OMA")){
      parse_error("We expect '</OMA>'.");
    }

    ret = new CMO_MONOMIAL32[mono.size()];
    mono.copyInto((Object[])ret);

    return ret;
  }

  private CMO_MONOMIAL32 parse_symb_Monom() throws IOException{
    Vector degree = new Vector();
    int[] array;

    exceptTokenTypeInParse(TT_StartTag);
    if(!attribute.equals("OMA")){
      parse_error("We expect '<OMA>'.");
    }

    if(!parse_symbol().equals("Monom")){
      parse_error("We expect '<Monom>'");
    }

    while(readNextToken() != TT_EndTag){
      pushbackLex();
      degree.addElement(parse_OMI());
    }

    if(!attribute.equals("OMA")){
      parse_error("We expect '<OMA>'.");
    }

    array = new int[degree.size()-1];
    for(int i=0;i<array.length;i++){
      array[i] = ((CMO_ZZ)degree.elementAt(i+1)).intValue();
    }

    //debug("monom: "+ new CMO_MONOMIAL32(array,(CMO_ZZ)degree.elementAt(0)));
    return new CMO_MONOMIAL32(array,(CMO_ZZ)degree.elementAt(0));
  }

  private CMO_LIST parse_objects() throws IOException{
    // 解析された object を LIST で返す
    Vector objects = new Vector();
    CMO[] array;

    while(readNextToken() != TT_EndTag){
      pushbackLex();
      objects.addElement(parse_object());
    }
    pushbackLex();

    objects.trimToSize();
    array = new CMO[objects.size()];
    objects.copyInto((Object[])array);

    debug("debug :"+ new CMO_LIST(array));
    return new CMO_LIST(array);
  }

  private String parse_symbol() throws IOException{
    // symbname を返す
    String ret = "";
    StreamTokenizer para;

    exceptTokenTypeInParse(TT_SingleTag);

    para = new StreamTokenizer(new StringReader(attribute));
    para.resetSyntax();
    para.wordChars('A','Z');
    para.wordChars('a','z');
    para.wordChars('0','9');
    para.wordChars('\u00A0','\u00FF');
    para.whitespaceChars('\u0000','\u0020');
    para.quoteChar('"');
    para.eolIsSignificant(false);
    para.lowerCaseMode(true);
    para.slashStarComments(false);
    para.slashSlashComments(false);
    para.ordinaryChar('=');

    if(para.nextToken() != StreamTokenizer.TT_WORD
       || !para.sval.equalsIgnoreCase("OMS")){
      parse_error("We expect '<OMS>', but '"+ para.sval +"'.");
    }

    if(para.nextToken() != StreamTokenizer.TT_WORD){
      parse_error("We expect 'name' or 'cd'.");
    }
    if(para.sval.equals("name")){
      if(para.nextToken() != '='){
	parse_error("We expect '='.");
      }
      if(para.nextToken() != '"'){
	parse_error("We expect '\"'.");
      }
      ret = parse_symbname(para.sval);

      if(para.nextToken() != StreamTokenizer.TT_WORD
	 || !para.sval.equals("cd")){
	parse_error("We expect 'cd'.");
      }
      if(para.nextToken() != '='){
	parse_error("We expect '='.");
      }
      if(para.nextToken() != '"'){
	parse_error("We expect '\"'.");
      }
      parse_cdname(para.sval);

    }else if(para.sval.equals("cd")){
      if(para.nextToken() != '='){
	parse_error("We expect '='.");
      }
      if(para.nextToken() != '"'){
	parse_error("We expect '\"'.");
      }
      parse_cdname(para.sval);

      if(para.nextToken() != StreamTokenizer.TT_WORD
	 || !para.sval.equals("name")){
	parse_error("We expect 'cd'.");
      }
      if(para.nextToken() != '='){
	parse_error("We expect '='.");
      }
      if(para.nextToken() != '"'){
	parse_error("We expect '\"'.");
      }
      ret = parse_symbname(para.sval);

    }else{
      parse_error("We expect 'name' or 'cd'.");
    }

    if(para.nextToken() != StreamTokenizer.TT_EOF){
      parse_error("We expect '/>'.");
    }

    return ret;
  }

  private CMO parse_integer() throws IOException{
    //Attribute integer = new Attribute(value);
    String str;
    int i=0;

    exceptTokenTypeInParse(TT_String);
    str = attribute;

    if(str.charAt(i)=='-'){
      i++;
    }
    if(str.length()>=i+2 && str.charAt(i)=='x'){ // for HEX
      String strhex = str.substring(0,i) + str.substring(i+1);
      for(i++;i<str.length();i++){
	if("0123456789ABCDEF".indexOf((int)str.charAt(i)) == -1){
	  parse_error("We expect integer.");
	}
      }
      // 10 進数に変換
      str = (new java.math.BigInteger(strhex,16)).toString();
    }else if(str.length()>=i+1){ // for DEC
      for(;i<str.length();i++){
	if("0123456789".indexOf((int)str.charAt(i)) == -1){
	  parse_error("We expect integer.");
	}
      }
    }else{
      parse_error("We expect integer.");
    }

    return new CMO_ZZ(str);
  }

  private CMO parse_variable() throws IOException{
    StreamTokenizer para;
    CMO ret;

    exceptTokenTypeInParse(TT_SingleTag);

    para = new StreamTokenizer(new StringReader(attribute));
    para.resetSyntax();
    para.wordChars('A','Z');
    para.wordChars('a','z');
    para.wordChars('0','9');
    para.wordChars('\u00A0','\u00FF');
    para.whitespaceChars('\u0000','\u0020');
    para.quoteChar('"');
    para.eolIsSignificant(false);
    para.lowerCaseMode(true);
    para.slashStarComments(false);
    para.slashSlashComments(false);
    para.ordinaryChar('=');

    if(para.nextToken() != StreamTokenizer.TT_WORD
       || !para.sval.equalsIgnoreCase("OMV")){
      parse_error("We expect '<OMV>'.");
    }

    if(para.nextToken() != StreamTokenizer.TT_WORD
       || !para.sval.equals("name")){
      parse_error("We expect 'name', but obtain '"+ para.sval +"'.");
    }
    if(para.nextToken() != '='){
      parse_error("We expect '='.");
    }
    if(para.nextToken() != '"'){
      parse_error("We expect '\"'.");
    }
    ret = new CMO_INDETERMINATE(parse_varname(para.sval));

    if(para.nextToken() != StreamTokenizer.TT_EOF){
      parse_error("We expect '/>'.");
    }

    return ret;
  }

  private String parse_varname(String str){
    String words = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      + "0123456789+='(),-./:?!#$%*;@[]^_`{|}";

    for(int i=0;i<str.length();i++){
      if(words.indexOf((int)str.charAt(i))==-1){
	parse_error("string \""+ str +"\" is not varname.");
      }
    }

    return str;
  }

  private String parse_cdname(String str){
    String alphabet = "abcdefghijklmnopqrstuvwxyz";
    int i=0;

    if(alphabet.indexOf((int)str.charAt(i))==-1){
      parse_error("cdname \""+ str +"\" must begin small alphabet.");
    }
    for(i++;i<str.length();i++){
      if((alphabet + "0123456789_").indexOf((int)str.charAt(i))==-1){
	parse_error("string \""+ str +"\" is not cdname.");
      }
    }

    return str;
  }

  private String parse_symbname(String str) throws IOException{
    String Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i=0;

    if(Alphabet.indexOf((int)str.charAt(i))==-1){
      parse_error("symbname \""+ str +"\" must begin alphabet.");
    }
    for(i++;i<str.length();i++){
      if((Alphabet + "0123456789_").indexOf((int)str.charAt(i))==-1){
	parse_error("string \""+ str +"\" is not symbname.");
      }
    }

    return str;
  }

  private void parse_error(String mesg) throws NumberFormatException{
    String ret;

    ret = mesg +"\n";
    ret += "error occuered near :";
    try{
      for(int i=0;i<10;i++){
	ret += (char)is.read();
      }
    }catch(IOException e){}
    ret += "\n";

    throw new NumberFormatException(ret);
  }

  private void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  public static void main(String[] argv) throws IOException{
    OM2OXM P = new OM2OXM();
    OpenXM asir;
    String host = "localhost";
    int CtrlPort = 1200,StreamPort = 1300;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
	System.out.println("");
	System.exit(0);
      }else if(argv[i].equals("-host")){
	host = argv[++i];
      }else if(argv[i].equals("-data")){
	StreamPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
	CtrlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
	System.err.println("unknown option :"+ argv[i]);
	System.exit(1);
      }
    }

    try{
      asir = new OpenXM(host,CtrlPort,StreamPort);
      asir.sendSM(new SM(SM.SM_mathcap));
    }catch(UnknownHostException e){
      System.err.println("host unknown.");
      System.err.println(e.getMessage());
      return;
    }catch(IOException e){
      System.err.println("connection failed.");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }

    P.asir = asir;
    new Thread(P).start();
    System.out.println("start");

    try{
      //P.value = new Attribute(System.in);

      //サーバ側から送信された文字列を受信します。
      while(true){
	try{
	  CMO obj = P.parse(System.in);
	  asir.send(obj);
	  asir.sendSM(new SM(SM.SM_popCMO));
	}catch(NumberFormatException e){
	  System.err.println(e.getMessage());
	}
      }

    }catch(IOException e){
      e.printStackTrace();
    }finally{
      System.out.println("breaking...");
    }
  }
}
