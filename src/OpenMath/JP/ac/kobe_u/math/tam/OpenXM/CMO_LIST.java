/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_LIST.java,v 1.10 2000/03/15 17:58:06 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_LIST extends CMO{
  private CMO ob[];

  public CMO_LIST(CMO[] src){
    ob = src;
  }

  public CMO[] getElements(){
    return ob;
  }

  public int DISCRIMINATOR(){
    return CMO.LIST;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    os.writeInt(ob.length);

    for(int i=0;i<ob.length;i++){
      ob[i].write(os);
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int len;
    CMO[] ob;

    len = is.readInt();
    ob = new CMO[len];

    for(int i=0;i<len;i++){
      ob[i] = CMO.receive(is);
    }

    return new CMO_LIST(ob);
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
