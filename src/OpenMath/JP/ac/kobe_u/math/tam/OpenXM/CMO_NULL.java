/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_NULL.java,v 1.3 2000/03/14 04:44:18 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_NULL extends CMO{
  public CMO_NULL(){}

  public int DISCRIMINATOR(){
    return CMO.NULL;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    return new CMO_NULL();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
