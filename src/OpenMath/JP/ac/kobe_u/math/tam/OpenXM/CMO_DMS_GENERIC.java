/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS_GENERIC.java,v 1.3 2000/03/14 05:38:50 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_DMS_GENERIC extends CMO{
  public CMO_DMS_GENERIC(){}

  public int DISCRIMINATOR(){
    return CMO.DMS_GENERIC;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_DMS_GENERIC();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS_GENERIC";
  }
}
