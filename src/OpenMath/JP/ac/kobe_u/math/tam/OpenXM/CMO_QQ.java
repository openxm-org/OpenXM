/**
 * CMO_QQ.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_QQ extends CMO{
  private CMO_ZZ a,b;

  CMO_QQ(){}

  CMO_QQ(CMO_ZZ src_a,CMO_ZZ src_b){
    a = src_a;
    b = src_b;
  }

  public int DISCRIMINATOR(){
    return CMO_QQ;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    a.send(os);
    b.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    a.receive(is);
    b.receive(is);

    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }

  public CMO_ZZ getBunshi(){
    return a;
  }

  public CMO_ZZ getBunbo(){
    return b;
  }
}
