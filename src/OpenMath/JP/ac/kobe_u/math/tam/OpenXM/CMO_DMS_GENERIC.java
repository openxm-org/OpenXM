/**
 * CMO_DMS_GENERIC.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_DMS_GENERIC extends CMO{
  public CMO_DMS_GENERIC(){}

  public int DISCRIMINATOR(){
    return CMO_DMS_GENERIC;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS_GENERIC";
  }
}
