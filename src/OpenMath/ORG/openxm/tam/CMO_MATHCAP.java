/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の MATHCAP 型を表します.
 */
final public class CMO_MATHCAP extends CMO{
  private CMO_LIST list;

  /**
   * CMO_LIST src を内容とする MATHCAP を作成します.
   * src は MATHCAP のセマンティックスを満たしていると仮定しています.
   */
  public CMO_MATHCAP(CMO_LIST src){
    list = src;
  }

  /**
   * 内容の LIST を得ます.
   */
  public CMO_LIST getList(){
    return list;
  }

  public int DISCRIMINATOR(){
    return CMO.MATHCAP;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    list.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO_LIST list = (CMO_LIST)CMO.receive(is);

    return new CMO_MATHCAP(list);
  }

  public String toCMOexpressionByObject(){
    return "CMO_MATHCAP,"+ list.toCMOexpression();
  }
}
