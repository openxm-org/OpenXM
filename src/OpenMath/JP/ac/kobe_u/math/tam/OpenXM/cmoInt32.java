/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class cmoInt32 extends CMO{
  private int value;

  public cmoInt32(int i){
    value = i;
  }

  public int intValue(){
    return value;
  }

  public int DISCRIMINATOR(){
    return CMO_INT32;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    os.writeInt(value);
  }

  protected static CMO receive(DataInputStream is) throws IOException{
    int value = is.readInt();

    return new cmoInt32(value);
  }

  public String toCMOexpressionByObject(){
    return "CMO_INT32,"+ value;
  }
}
