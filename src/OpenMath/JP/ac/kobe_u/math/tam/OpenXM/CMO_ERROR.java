/**
 * CMO_ERROR.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_ERROR extends CMO{
  CMO_ERROR(){}

  public int DISCRIMINATOR(){
    return CMO_ERROR;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR";
  }
}
