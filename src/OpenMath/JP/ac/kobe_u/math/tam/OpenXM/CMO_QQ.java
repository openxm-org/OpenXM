/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_QQ.java,v 1.4 2000/01/18 16:54:21 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

public class CMO_QQ extends CMO{
  private CMO_ZZ a,b;

  CMO_QQ(){}

  CMO_QQ(CMO_ZZ src_a,CMO_ZZ src_b){
    a = src_a;
    b = src_b;
  }

  public CMO_ZZ getBunshi(){
    return a;
  }

  public CMO_ZZ getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO_QQ;
  }

  private int sign(int a){
    if(a>0){
      return 1;
    }else if(a<0){
      return -1;
    }
    return 0;
  }

  protected void sendByObject(DataOutputStream os) throws IOException{
    a.sendByObject(os);
    b.sendByObject(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    a = (CMO_ZZ) new CMO_ZZ().receiveByObject(is);
    b = (CMO_ZZ) new CMO_ZZ().receiveByObject(is);

    return this;
  }

  protected String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
