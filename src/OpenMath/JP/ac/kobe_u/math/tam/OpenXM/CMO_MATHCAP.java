/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_MATHCAP extends CMO{
  private CMO_LIST list;

  public CMO_MATHCAP(){}

  public CMO_MATHCAP(CMO_LIST src){
    list = src;
  }

  public int DISCRIMINATOR(){
    return CMO_MATHCAP;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    list.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int len;

    list = (CMO_LIST)receive(is);
    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_MATHCAP,"+ list.toCMOexpression();
  }
}
