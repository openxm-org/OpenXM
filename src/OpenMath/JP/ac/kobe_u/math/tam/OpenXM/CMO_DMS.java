/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS.java,v 1.3 2000/03/14 05:38:50 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_DMS extends CMO{
  public CMO_DMS(){}

  public int DISCRIMINATOR(){
    return CMO.DMS;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_DMS();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS";
  }
}
