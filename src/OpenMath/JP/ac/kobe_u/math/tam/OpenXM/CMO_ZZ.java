/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

public class CMO_ZZ extends CMO{
  private BigInteger num;

  public CMO_ZZ(){}

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

	len = 4 * is.readInt();
	bignum = new byte[Math.abs(len)];
	is.readFully(bignum,0,Math.abs(len));

	num = new BigInteger(sign(len),bignum);

	return this;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
	int len = (this.num.bitLength()+7)/8;
	byte[] bignum = this.num.abs().toByteArray();

    //System.out.println(""+ len +":"+ bignum.length +":"+ bignum[0]);
    if(len==0){
      os.writeInt(0);
    }else{
      os.writeInt((Math.abs(len)+3)/4*sign(len));
	  for(int i=0;i<(4-Math.abs(len)%4)%4;i++){
		os.write(0);
	  }
      os.write(bignum);
    }
  }

  public String toCMOexpressionByObject() throws NumberFormatException{
    return "CMO_ZZ,"+ this.num;
  }
}
