/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_BIGFLOAT.java,v 1.3 2000/01/18 16:54:21 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_BIGFLOAT extends CMO{
  // a * 2^e
  private CMO_ZZ a,e;

  CMO_BIGFLOAT(){}

  CMO_BIGFLOAT(CMO_ZZ a,CMO_ZZ e){
    this.a = a;
    this.e = e;
  }

  public int DISCRIMINATOR(){
    return CMO_BIGFLOAT;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    a.sendByObject(os);
    e.sendByObject(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    a = (CMO_ZZ) new CMO_ZZ().receiveByObject(is);
    e = (CMO_ZZ) new CMO_ZZ().receiveByObject(is);

    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_BIGFLOAT,"+ a.toCMOexpression() +","+ e.toCMOexpression();
  }

  public CMO_ZZ getSyosubu(){
    return a;
  }

  public CMO_ZZ getShisubu(){
    return e;
  }
}
