/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_NULL.java,v 1.4 2000/03/14 05:02:35 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_NULL extends CMO{
  public CMO_NULL(){}

  public int DISCRIMINATOR(){
    return CMO.NULL;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_NULL();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
