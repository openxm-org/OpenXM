/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_DATUM.java,v 1.3 2000/03/14 04:56:24 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_DATUM extends CMO{
  private byte data[];

  CMO_DATUM(byte in[]){
    data = in;
  }

  public int DISCRIMINATOR(){
    return DATUM;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
    os.writeInt(data.length);
    for(int i=0;i<data.length;i++){
      os.writeByte(data[i]);
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int len;
    byte[] data;

    len = is.readInt();
    data = new byte[len];

    for(int i=0;i<len;i++){
      data[i] = is.readByte();
    }

    return new CMO_DATUM(data);
  }

  public String toCMOexpressionByObject(){
    String str = "";

    for(int i=0;i<data.length;i++){
      str += ","+ data[i];
    }

    return "CMO_DATUM,"+ data.length + str;
  }
}
