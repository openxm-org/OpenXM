/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_NULL extends CMO{
  public CMO_NULL(){}

  public int DISCRIMINATOR(){
    return CMO_NULL;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
