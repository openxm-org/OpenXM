/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_INT32.java,v 1.4 2000/03/14 05:02:35 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_INT32 extends CMO{
  private int value;

  public CMO_INT32(int i){
    value = i;
  }

  public int DISCRIMINATOR(){
    return CMO.INT32;
  }

  public int intValue(){
    return value;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
    os.writeInt(value);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int value = is.readInt();

    return new CMO_INT32(value);
  }

  public String toCMOexpressionByObject(){
    return "CMO_INT32,"+ value;
  }
}
