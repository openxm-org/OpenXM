/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_RATIONAL.java,v 1.2 2000/01/18 16:54:21 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_RATIONAL extends CMO{
  private CMO a,b;

  CMO_RATIONAL(){}

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
    return CMO_RATIONAL;
  }

  protected void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    a.send(os);
    b.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    a = CMO.receive(is);
    b = CMO.receive(is);

    return this;
  }

  protected String toCMOexpressionByObject(){
    return "CMO_RATIONAL,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
