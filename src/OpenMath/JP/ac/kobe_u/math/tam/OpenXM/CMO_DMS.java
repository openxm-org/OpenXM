/**
 * CMO_DMS.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_DMS extends CMO{
  public CMO_DMS(){}

  public int DISCRIMINATOR(){
    return CMO_DMS;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS";
  }
}
