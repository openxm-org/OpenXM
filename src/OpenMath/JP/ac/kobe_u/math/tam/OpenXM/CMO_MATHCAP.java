/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_MATHCAP.java,v 1.2 2000/01/20 18:14:33 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_MATHCAP extends CMO{
  private CMO_LIST list;

  public CMO_MATHCAP(){}

  public CMO_MATHCAP(CMO_LIST src){
    list = src;
  }

  public CMO_LIST getList(){
    return list;
  }

  public int DISCRIMINATOR(){
    return CMO_MATHCAP;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    list.write(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int len;

    list = (CMO_LIST)receive(is);
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_MATHCAP,"+ list.toCMOexpression();
  }
}
