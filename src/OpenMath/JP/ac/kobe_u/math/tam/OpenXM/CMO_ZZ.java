/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ZZ.java,v 1.5 1999/12/09 00:32:37 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

public class CMO_ZZ extends CMO{
  private BigInteger num;

  public CMO_ZZ(){}

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
    int len;
    byte[] bignum;

    len = is.readInt();
    bignum = new byte[4 * Math.abs(len)];
    for(int i=Math.abs(len);--i>=0;){
      is.readFully(bignum,i*4,4);
    }

    num = new BigInteger(sign(len),bignum);

    return this;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    if(this.num.signum()==0){
      os.writeInt(0);
    }else{
      byte[] bignum = this.num.abs().toByteArray();
      int len = bignum.length;

      while(bignum[0] == 0){
	byte[] tmp = new byte[len-1];

	for(int i=0;i<tmp.length;i++){
	  tmp[i] = bignum[i+1];
	}
	bignum = tmp;
	len = bignum.length;
      }

      //System.err.println("CMO_ZZ:"+ len +":"+ bignum.length +":"+ bignum[0]);
      os.writeInt((len+3)/4*(this.num.signum()));
      for(int i=len;(i-=4)>=0;){
	os.write(bignum,i,4);
      }
      for(int i=0;i<(4-len%4)%4;i++){
	os.write(0);
      }
      os.write(bignum,0,len%4);
    }
  }

  public String toCMOexpressionByObject() throws NumberFormatException{
    return "CMO_ZZ,"+ this.num;
  }
}
