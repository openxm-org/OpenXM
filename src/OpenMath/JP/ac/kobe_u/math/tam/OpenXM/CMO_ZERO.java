/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_ZERO extends CMO{
  CMO_ZERO(){}

  public int DISCRIMINATOR(){
    return CMO_ZERO;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_ZERO";
  }
}
