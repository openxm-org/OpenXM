/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR2.java,v 1.2 1999/11/07 21:22:03 tam Exp $
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

  public void sendByObject(DataOutputStream os) throws IOException{
    ob.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    ob = CMO.receive(is);
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
