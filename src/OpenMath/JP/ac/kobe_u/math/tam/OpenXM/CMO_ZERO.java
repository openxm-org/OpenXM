/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_ZERO.java,v 1.3 2000/03/14 05:38:51 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_ZERO extends CMO{
  CMO_ZERO(){}

  public int DISCRIMINATOR(){
    return CMO.ZERO;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_ZERO();
  }

  public String toCMOexpressionByObject(){
    return "CMO_ZERO";
  }
}
