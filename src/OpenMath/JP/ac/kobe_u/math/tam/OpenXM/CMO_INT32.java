/**
 * CMO_INT32.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_INT32 extends CMO{
  private int value;

  public CMO_INT32(){}

  public CMO_INT32(int i){
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

  protected CMO receiveByObject(DataInputStream is) throws IOException{
	value = is.readInt();
	return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_INT32,"+ value;
  }
}
