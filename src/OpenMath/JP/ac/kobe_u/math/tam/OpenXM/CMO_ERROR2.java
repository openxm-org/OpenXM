/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR2.java,v 1.4 2000/01/20 18:14:33 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_ERROR2 extends CMO{
  private CMO ob = new CMO_NULL();

  public CMO_ERROR2(){}

  public CMO_ERROR2(CMO src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO_ERROR2;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    ob.write(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    ob = CMO.receive(is);
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
