/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ZERO.java,v 1.2 1999/11/07 21:22:03 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_ZERO extends CMO{
  CMO_ZERO(){}

  public int DISCRIMINATOR(){
    return CMO.ZERO;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    return new CMO_ZERO();
  }

  public String toCMOexpressionByObject(){
    return "CMO_ZERO";
  }
}
