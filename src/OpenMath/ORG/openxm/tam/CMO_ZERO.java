/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_ZERO.java,v 1.1 2000/09/12 07:05:07 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の ZERO 型を表します.
 */
final public class CMO_ZERO extends CMO{
  /**
   * ZERO を作成します.
   */
  CMO_ZERO(){}

  public int DISCRIMINATOR(){
    return CMO.ZERO;
  }

  public void sendByObject(OpenXMstream os) throws IOException{
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    return new CMO_ZERO();
  }

  public String toCMOexpressionByObject(){
    return "CMO_ZERO";
  }
}
