/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_BIGFLOAT.java,v 1.5 2000/03/14 05:38:50 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_BIGFLOAT extends CMO{
  // a * 2^e
  private CMO_ZZ a,e;

  CMO_BIGFLOAT(CMO_ZZ a,CMO_ZZ e){
    this.a = a;
    this.e = e;
  }

  public int DISCRIMINATOR(){
    return CMO.BIGFLOAT;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
    a.sendByObject(os);
    e.sendByObject(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO_ZZ a,e;

    a = (CMO_ZZ) CMO_ZZ.receive(is);
    e = (CMO_ZZ) CMO_ZZ.receive(is);

    return new CMO_BIGFLOAT(a,e);
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
