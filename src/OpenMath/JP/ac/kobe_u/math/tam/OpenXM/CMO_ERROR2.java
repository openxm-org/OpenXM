/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR2.java,v 1.6 2000/03/14 04:44:17 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_ERROR2 extends CMO{
  private CMO ob = new CMO_NULL();

  public CMO_ERROR2(CMO src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO.ERROR2;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    ob.write(os);
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    return new CMO_ERROR2(CMO.receive(is));
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
