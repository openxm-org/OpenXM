/**
 * $OpenXM$
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

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_ZERO();
  }

  public String toCMOexpressionByObject(){
    return "CMO_ZERO";
  }
}
