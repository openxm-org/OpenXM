/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_STRING.java,v 1.4 2000/03/14 05:02:35 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_STRING extends CMO{
  private String str = null;

  public CMO_STRING(){
    str = null;
  }

  public CMO_STRING(String a){
    str = a;
  }

  public String getString(){
    return str;
  }

  public int DISCRIMINATOR(){
    return CMO.STRING;
  }

  protected void sendByObject(OpenXMconnection os) throws IOException{
    byte[] buf = str.getBytes();

    os.writeInt(buf.length);
    for(int i=0;i<buf.length;i++){
      os.writeByte(buf[i]);
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int len;
    byte[] buf=null;

    len=is.readInt();
    if(len==0){
      return new CMO_STRING("");
    }

    buf = new byte[len];
    for(int i=0;i<len;i++){
      buf[i] = is.readByte();
    }

    return new CMO_STRING(new String(buf));
  }

  protected String toCMOexpressionByObject(){
    return "CMO_STRING,"+ str.length() +","+ str;
  }
}
