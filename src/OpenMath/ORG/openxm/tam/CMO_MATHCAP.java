/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_MATHCAP.java,v 1.2 2000/09/13 06:32:42 tam Exp $
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

    public boolean allowQ (int[] datacap) {
		return CMO.allowQ_tag(datacap, CMO.MATHCAP) && list.allowQ(datacap);
    }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    list.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    CMO_LIST list = (CMO_LIST)CMO.receive(is);

    return new CMO_MATHCAP(list);
  }

  public String toCMOexpressionByObject(){
    return "CMO_MATHCAP,"+ list.toCMOexpression();
  }
}
