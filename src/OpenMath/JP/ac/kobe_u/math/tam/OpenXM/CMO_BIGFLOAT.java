/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_BIGFLOAT.java,v 1.2 1999/11/15 06:19:27 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_BIGFLOAT extends CMO{
  // a * 2^e
  private CMO_ZZ a,e;

  CMO_BIGFLOAT(){}

  CMO_BIGFLOAT(CMO_ZZ a,CMO_ZZ e){
    this.a = a;
    this.e = e;
  }

  public int DISCRIMINATOR(){
    return CMO_BIGFLOAT;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    a.send(os);
    e.send(os);
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
    e = (CMO_ZZ)tmp;

    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_BIGFLOAT,"+ a.toCMOexpression() +","+ e.toCMOexpression();
  }

  public CMO_ZZ getSyosubu(){
    return a;
  }

  public CMO_ZZ getShisubu(){
    return e;
  }
}
