/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ZZ.java,v 1.9 2000/03/16 09:51:32 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

final public class CMO_ZZ extends CMO{
  private BigInteger num;

  public CMO_ZZ(BigInteger num){
    this.num = num;
  }

  public CMO_ZZ(String str) throws NumberFormatException{
    if(str.startsWith("+")){
      this.num = new BigInteger(str.substring(1));
    }else{
      this.num = new BigInteger(str);
    }
  }

  public int intValue(){
    return num.intValue();
  }

  public BigInteger BigIntValue(){
    return num;
  }

  public int DISCRIMINATOR(){
    return CMO.ZZ;
  }

  static private int sign(int a){
    if(a>0){
      return 1;
    }else if(a<0){
      return -1;
    }
    return 0;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
    if(this.num.signum()==0){
      os.writeInt(0);
    }else{
      int len = (this.num.abs().getLowestSetBit()+31)/32;

      //System.err.println("CMO_ZZ:"+ len +":"+ bignum.length +":"+ bignum[0]);
      os.writeInt(this.num.signum()*len);

      for(BigInteger a = this.num.abs();
	  a.compareTo(new BigInteger("0"))>0;
	  a = a.divide(new BigInteger("4294967296"))){
	os.writeInt(a.remainder(new BigInteger("4294967296")).intValue());
      }
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int len;
    BigInteger a = new BigInteger("0");

    len = is.readInt();
    for(int i=0;i<Math.abs(len);i++){
      a = a.add(new BigInteger("4294967296").pow(i).multiply(new BigInteger(""+is.readInt())));
    }

    if(len<0){
      a = a.negate();
    }

    return new CMO_ZZ(a);
  }

  public String toCMOexpressionByObject() throws NumberFormatException{
    return "CMO_ZZ,"+ this.num;
  }
}
