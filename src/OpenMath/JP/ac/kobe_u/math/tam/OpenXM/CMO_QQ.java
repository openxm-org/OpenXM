/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_QQ.java,v 1.3 1999/11/21 20:38:41 tam Exp $
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

  public CMO_ZZ getBunshi(){
    return a;
  }

  public CMO_ZZ getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO_QQ;
  }

  protected void sendByObject(DataOutputStream os) throws IOException{
    a.send(os);
    b.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_ZZ)){
      System.err.println("not CMOobject");
    }
    a = (CMO_ZZ)tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_ZZ)){
      System.err.println("not CMOobject");
    }
    b = (CMO_ZZ)tmp;

    return this;
  }

  protected String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
