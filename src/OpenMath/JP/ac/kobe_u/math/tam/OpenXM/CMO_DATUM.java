/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_DATUM extends CMO{
  private byte data[];

  CMO_DATUM(){}

  CMO_DATUM(byte in[]){
    data = in;
  }

  public int DISCRIMINATOR(){
    return CMO_DATUM;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    os.writeInt(data.length);
    os.write(data,0,data.length);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int len;

    len = is.readInt();
    data = new byte[len];

    for(int i=0;i<len;i++){
      data[i] = is.readByte();
    }

    return this;
  }

  public String toCMOexpressionByObject(){
    String str = "";

    for(int i=0;i<data.length;i++){
      str += ","+ data[i];
    }

    return "CMO_DATUM,"+ data.length + str;
  }
}
