/**
 * CMO_ZZ.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

public class CMO_ZZ extends CMO{
  int len=0;
  private byte[] bignum;

  public CMO_ZZ(){}

  public CMO_ZZ(String str) throws NumberFormatException{
    BigInteger a;

    if(str.startsWith("+")){
      a = new BigInteger(str.substring(1));
    }else{
      a = new BigInteger(str);
    }

    bignum = a.abs().toByteArray();
    len = bignum.length * a.signum();
  }

  public int DISCRIMINATOR(){
    return CMO_ZZ;
  }

  private int sign(int a){
    if(a>0){
      return 1;
    }else if(a<0){
      return -1;
    }
    return 0;
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
	len = is.readInt();
	is.readFully(bignum,0,len*sign(len));
	return this;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    //System.out.println(""+ len +":"+ bignum.length +":"+ bignum[0]);
    if(len==0){
      os.writeInt(0);
    }else{
      os.writeInt(len);
      os.write(bignum);
    }
  }

  public String toCMOexpressionByObject() throws NumberFormatException{
    return "CMO_ZZ,"+ (new BigInteger(sign(len),bignum).toString());
  }
}
