/**
 * $OpenXM$
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

  public static CMO receive(DataInputStream is) throws IOException{
    return new cmoNull();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
