/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_STRING.java,v 1.3 1999/11/17 13:23:16 tam Exp $
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

  static protected CMO receive(DataInputStream is) throws IOException{
    int len;
    byte[] buf=null;

    len=is.readInt();
    if(len==0){
      return new CMO_STRING("");
    }

    buf = new byte[len];
    is.readFully(buf,0,len);

    return new CMO_STRING(new String(buf));
  }

  protected void sendByObject(DataOutputStream os) throws IOException{
    os.writeInt(str.getBytes().length);
    os.write(str.getBytes());
  }

  protected String toCMOexpressionByObject(){
    return "CMO_STRING,"+ str.length() +","+ str;
  }
}
