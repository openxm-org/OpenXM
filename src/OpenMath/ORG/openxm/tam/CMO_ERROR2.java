/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_ERROR2.java,v 1.1 2000/09/12 07:05:05 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の ERROR2 型を表します.
 */
final public class CMO_ERROR2 extends CMO{
  private CMO ob = new CMO_NULL();

  /**
   * CMO src を内容とする ERROR2 を作成します.
   */
  public CMO_ERROR2(CMO src){
    ob = src;
  }

  public int DISCRIMINATOR(){
    return CMO.ERROR2;
  }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    ob.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    return new CMO_ERROR2(CMO.receive(is));
  }

  public String toCMOexpressionByObject(){
    return "CMO_ERROR2,"+ ob.toCMOexpression();
  }
}
