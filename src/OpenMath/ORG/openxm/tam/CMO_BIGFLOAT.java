/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の BIGFLOAT 型を表します.
 */
final public class CMO_BIGFLOAT extends CMO{
  // a * 2^e
  private CMO_ZZ a,e;

  /**
   * (a * 2^e) を表す BIGFLOAT を作成します.
   */
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

  /**
   * 小数部を得ます.
   */
  public CMO_ZZ getSyosubu(){
    return a;
  }

  /**
   * 指数部を得ます.
   */
  public CMO_ZZ getShisubu(){
    return e;
  }
}
