/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS_GENERIC.java,v 1.2 1999/11/07 21:22:03 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_DMS_GENERIC extends CMO{
  public CMO_DMS_GENERIC(){}

  public int DISCRIMINATOR(){
    return CMO.DMS_GENERIC;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    return new CMO_DMS_GENERIC();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS_GENERIC";
  }
}
