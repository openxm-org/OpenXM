/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO.java,v 1.17 2000/03/14 05:38:50 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

abstract public class CMO extends OXbody{
  public static int[] mathcap = null;

  final public static int LARGEID     = 0x7f000000;
  final public static int ERROR   = ( LARGEID +1 );
  final public static int ERROR2  = ( LARGEID +2 );
  final public static int NULL    = 1;
  final public static int INT32   = 2;
  final public static int DATUM   = 3;
  final public static int STRING  = 4;
  final public static int MATHCAP = 5;
  final public static int LIST    = 17;

  final public static int MONOMIAL32         = 19;
  final public static int ZZ                 = 20;
  final public static int QQ                 = 21;
  final public static int ZERO               = 22;
  final public static int DMS                = 23;
  final public static int DMS_GENERIC        = 24;
  final public static int DMS_OF_N_VARIABLES = 25;
  final public static int RING_BY_NAME       = 26;

  final public static int RECURSIVE_POLYNOMIAL       = 27;
  final public static int DISTRIBUTED_POLYNOMIAL     = 31;
  final public static int POLYNOMIAL_IN_ONE_VARIABLE = 33;

  final public static int RATIONAL                   = 34;

  final public static int CMO_64BIT_MACHINE_DOUBLE           = 40;
  final public static int ARRAY_OF_64BIT_MACHINE_DOUBLE  = 41;
  final public static int CMO_128BIT_MACHINE_DOUBLE          = 42;
  final public static int ARRAY_OF_128BIT_MACHINE_DOUBLE = 43;

  final public static int BIGFLOAT          = 50;
  final public static int IEEE_DOUBLE_FLOAT = 51;

  final public static int INDETERMINATE      = 60;
  final public static int TREE               = 61;
  final public static int LAMBDA             = 62;

  final public static int PRIVATE = 0x7fff0000;

  public CMO(){}

  public CMO(InputStream is) throws IOException{
    //DataInputStream is;
  }

  abstract protected int DISCRIMINATOR();

  public int getDISCRIMINATOR(){
    return this.DISCRIMINATOR();
  }

  abstract protected void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation;

  final public void write(OpenXMconnection os)
       throws IOException,MathcapViolation{
    write(new DataOutputStream(os.os));
  }

  final protected void write(DataOutputStream os)
       throws IOException,MathcapViolation{
    if(mathcap != null){ // check mathcap
      int i=0;

      for(;i<mathcap.length;i++){
	if(mathcap[i] == this.DISCRIMINATOR()){
	  break;
	}
      }
      if(i>=mathcap.length){
	throw new MathcapViolation(this.toCMOexpression());
      }
    }
    os.writeInt(this.DISCRIMINATOR());
    this.sendByObject(os);
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    int a = 0;

    a = is.readInt();

    switch(a){
    case CMO.ERROR2:
      return CMO_ERROR2.receive(is);

    case CMO.NULL:
      return CMO_NULL.receive(is);

    case CMO.INT32:
      return CMO_INT32.receive(is);

    case CMO.DATUM:
      return CMO_DATUM.receive(is);

    case CMO.STRING:
      return CMO_STRING.receive(is);

    case CMO.MATHCAP:
      return CMO_MATHCAP.receive(is);

    case CMO.LIST:
      return CMO_LIST.receive(is);

    case CMO.MONOMIAL32:
      return CMO_MONOMIAL32.receive(is);

    case CMO.ZZ:
      return CMO_ZZ.receive(is);

    case CMO.QQ:
      return CMO_QQ.receive(is);

    case CMO.ZERO:
      return CMO_ZERO.receive(is);

    case CMO.DMS:
      return CMO_DMS.receive(is);

    case CMO.DMS_GENERIC:
      return CMO_DMS_GENERIC.receive(is);

    case CMO.RECURSIVE_POLYNOMIAL:
      return CMO_RECURSIVE_POLYNOMIAL.receive(is);

    case CMO.DISTRIBUTED_POLYNOMIAL:
      return CMO_DISTRIBUTED_POLYNOMIAL.receive(is);

    case CMO.POLYNOMIAL_IN_ONE_VARIABLE:
      return CMO_POLYNOMIAL_IN_ONE_VARIABLE.receive(is);

    case CMO.RATIONAL:
      return CMO_RATIONAL.receive(is);

    case CMO.BIGFLOAT:
      return CMO_BIGFLOAT.receive(is);

    case CMO.INDETERMINATE:
      return CMO_INDETERMINATE.receive(is);

    case CMO.TREE:
      return CMO_TREE.receive(is);

    default:
      System.err.println("unknown CMO type("+ a +")");
    }
    return null;
  }

  abstract protected String toCMOexpressionByObject();

  final public String toCMOexpression(){
    return "("+ this.toCMOexpressionByObject() +")";
  }

  final public String toOXexpression(){
    return toCMOexpression();
  }

  final public String toString(){
    return this.toCMOexpression();
  }
}
