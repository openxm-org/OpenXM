/*
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/SM.java,v 1.4 2000/02/21 03:48:22 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

final public class SM extends OXbody{
  private int SMcode;

  // SMobject/Basic 0 or 1
  final public static int SM_popSerializedLocalObject   = 258;
  final public static int SM_popCMO                     = 262;
  final public static int SM_popString                  = 263;

  final public static int SM_mathcap                    = 264;
  final public static int SM_pops                       = 265;
  final public static int SM_setName                    = 266;
  final public static int SM_evalName                   = 267;
  final public static int SM_executeStringByLocalParser = 268;
  final public static int SM_executeFunction            = 269;
  final public static int SM_beginBlock                 = 270;
  final public static int SM_endBlock                   = 271;
  final public static int SM_shutdown                   = 272;
  final public static int SM_setMathCap                 = 273;
  final public static int SM_executeStringByLocalParserInBatchMode = 274;

  final public static int SM_control_kill               = 1024;
  final public static int SM_control_reset_connection   = 1030;

  final public static int SM_PRIVATE                    = 0x7fff0000;

  public SM(int code){
    SMcode = code;
  }

  public SM(OpenXMconnection is) throws java.io.IOException{
    SMcode = is.readInt();
  }

  public int getCode(){
	return SMcode;
  }

  public void write(OpenXMconnection os) throws java.io.IOException{
    os.writeInt(SMcode);
  }

  public static OXbody receive(OpenXMconnection is) throws java.io.IOException{
    return new SM(is);
  }

  public String toString(){
    String val;

    switch(SMcode){
    case 258:
      val = "SM_popSerializedLocalObject";
      break;

    case 262:
      val = "SM_popCMO";
      break;

    case 263:
      val = "SM_popString";
      break;

    case 264:
      val = "SM_mathcap";
      break;

    case 265:
      val = "SM_pops";
      break;

    case 266:
      val = "SM_setName";
      break;

    case 267:
      val = "SM_evalName";
      break;

    case 268:
      val = "SM_executeStringByLocalParser";
      break;

    case 269:
      val = "SM_executeFunction";
      break;

    case 270:
      val = "SM_beginBlock";
      break;

    case 271:
      val = "SM_endBlock";
      break;

    case 272:
      val = "SM_shutdown";
      break;

    case 273:
      val = "SM_setMathCap";
      break;

    case 274:
      val = "SM_executeStringByLocalParserInBatchMode";
      break;

    case 1024:
      val = "SM_control_kill";
      break;

    case 1030:
      val = "SM_control_reset_connection";
      break;

    default:
      val = "";
    }
    return val;
  }

  public String toOXexpression(){
    return "";
  }
}
