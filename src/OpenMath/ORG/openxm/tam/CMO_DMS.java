/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の DMS 型を表します.
 */
final public class CMO_DMS extends CMO{
  /**
   * DMS を作成します.
   */
  public CMO_DMS(){}

  public int DISCRIMINATOR(){
    return CMO.DMS;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_DMS();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS";
  }
}
