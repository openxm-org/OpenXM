/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS.java,v 1.2 1999/11/07 21:22:03 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_DMS extends CMO{
  public CMO_DMS(){}

  public int DISCRIMINATOR(){
    return CMO.DMS;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    return new CMO_DMS();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS";
  }
}
