/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/cmoError2.java,v 1.1 2000/03/12 14:24:22 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class cmoError2 extends CMO{
  private CMO ob = new CMO_NULL();

  public cmoError2(CMO src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO_ERROR2;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    ob.write(os);
  }

  protected static CMO receive(DataInputStream is) throws IOException{
    return new cmoError2(CMO.receive(is));
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
