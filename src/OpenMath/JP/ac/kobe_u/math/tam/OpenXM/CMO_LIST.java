/**
 * CMO_LIST.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_LIST extends CMO{
  private CMO ob[];

  public CMO_LIST(){}

  public CMO_LIST(CMO[] src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO_LIST;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    os.writeInt(ob.length);

    for(int i=0;i<ob.length;i++){
      ob[i].send(os);
    }
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int len;

    len = is.readInt();
    ob = new CMO[len];

    for(int i=0;i<len;i++){
      ob[i] = receive(is);
    }

    return this;
  }

  public String toCMOexpressionByObject(){
    String str = "";

    for(int i=0;i<ob.length;i++){
	  //System.err.print(":"+ ob[i]);
      str += ","+ ob[i].toCMOexpression();
    }

    return "CMO_LIST,"+ ob.length + str;
  }
}
