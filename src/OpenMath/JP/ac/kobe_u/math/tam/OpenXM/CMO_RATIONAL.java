/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_RATIONAL.java,v 1.4 2000/02/21 03:48:22 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_RATIONAL extends CMO{
  private CMO a,b;

  CMO_RATIONAL(CMO src_a,CMO src_b){
    a = src_a;
    b = src_b;
  }

  public CMO getBunshi(){
    return a;
  }

  public CMO getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO.RATIONAL;
  }

  protected void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    a.write(os);
    b.write(os);
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    CMO a,b;

    a = CMO.receive(is);
    b = CMO.receive(is);

    return new CMO_RATIONAL(a,b);
  }

  protected String toCMOexpressionByObject(){
    return "CMO_RATIONAL,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
