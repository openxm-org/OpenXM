/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_QQ.java,v 1.6 2000/01/21 12:17:50 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

final public class CMO_QQ extends CMO{
  private CMO_ZZ a,b;

  public CMO_QQ(CMO_ZZ a,CMO_ZZ b){
    this.a = a;
    this.b = b;
  }

  public CMO_ZZ getBunshi(){
    return a;
  }

  public CMO_ZZ getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO.QQ;
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

  static protected CMO receive(DataInputStream is) throws IOException{
    CMO_ZZ a,b;

    a = (CMO_ZZ)CMO_ZZ.receive(is);
    b = (CMO_ZZ)CMO_ZZ.receive(is);

    return new CMO_QQ(a,b);
  }

  protected String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
