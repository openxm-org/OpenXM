/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_RATIONAL.java,v 1.5 2000/03/14 14:12:34 tam Exp $
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

  protected void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    a.write(os);
    b.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO a,b;

    a = CMO.receive(is);
    b = CMO.receive(is);

    return new CMO_RATIONAL(a,b);
  }

  protected String toCMOexpressionByObject(){
    return "CMO_RATIONAL,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
