/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_QQ.java,v 1.2 2000/09/13 06:32:43 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.math.BigInteger;

/**
 * CMO 形式の QQ 型を表します.
 */
final public class CMO_QQ extends CMO{
  private CMO_ZZ a,b;

  /**
   * (a/b) を表す QQ 型を表します.
   * セマンティクスのチェックは行なわれません.
   */
  public CMO_QQ(CMO_ZZ a,CMO_ZZ b){
    this.a = a;
    this.b = b;
  }

  /**
   * 分子を得ます.
   */
  public CMO_ZZ getBunshi(){
    return a;
  }

  /**
   * 分母を得ます.
   */
  public CMO_ZZ getBunbo(){
    return b;
  }

  public int DISCRIMINATOR(){
    return CMO.QQ;
  }

    public boolean allowQ (int[] datacap) {
		return CMO.allowQ_tag(datacap, DISCRIMINATOR()) && a.allowQ(datacap);
    }

  private int sign(int a){
    if(a>0){
      return 1;
    }else if(a<0){
      return -1;
    }
    return 0;
  }

  protected void sendByObject(OpenXMstream os) throws IOException{
    a.sendByObject(os);
    b.sendByObject(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    CMO_ZZ a,b;

    a = (CMO_ZZ)CMO_ZZ.receive(is);
    b = (CMO_ZZ)CMO_ZZ.receive(is);

    return new CMO_QQ(a,b);
  }

  protected String toCMOexpressionByObject(){
    return "CMO_QQ,"+ a.toCMOexpression() +","+ b.toCMOexpression();
  }
}
