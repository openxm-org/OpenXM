/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_MATHCAP.java,v 1.4 2000/03/14 05:06:47 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_MATHCAP extends CMO{
  private CMO_LIST list;

  public CMO_MATHCAP(CMO_LIST src){
    list = src;
  }

  public CMO_LIST getList(){
    return list;
  }

  public int DISCRIMINATOR(){
    return CMO.MATHCAP;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    list.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO_LIST list = (CMO_LIST)receive(is);

    return new CMO_MATHCAP(list);
  }

  public String toCMOexpressionByObject(){
    return "CMO_MATHCAP,"+ list.toCMOexpression();
  }
}
