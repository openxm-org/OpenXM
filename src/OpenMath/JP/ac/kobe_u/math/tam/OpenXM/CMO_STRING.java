/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_STRING extends CMO{
  private String str = null;

  public CMO_STRING(){
    str = null;
  }

  public CMO_STRING(String a){
    str = a;
  }

  public int DISCRIMINATOR(){
	return CMO_STRING;
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int len;
    byte[] buf=null;

    len=is.readInt();
    if(len==0){
      return new CMO_STRING("");
    }

    buf = new byte[len];
    is.read(buf,0,len);

    return new CMO_STRING(new String(buf));
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    os.writeInt(str.getBytes().length);
    os.write(str.getBytes());
  }

  public String toCMOexpressionByObject(){
    return "CMO_STRING,"+ str.length() +","+ str;
  }

  public String getString(){
    return str;
  }
}
