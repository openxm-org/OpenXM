/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_BIGFLOAT.java,v 1.1 1999/11/04 19:57:22 tam Exp $
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
    a.send(os);
    e.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    a.receive(is);
    e.receive(is);

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
