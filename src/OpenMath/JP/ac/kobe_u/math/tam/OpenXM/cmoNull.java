/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/cmoNull.java,v 1.1 2000/03/12 14:24:22 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class cmoNull extends CMO{
  public cmoNull(){}

  public int DISCRIMINATOR(){
    return CMO_NULL;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  protected static CMO receive(DataInputStream is) throws IOException{
    return new cmoNull();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
