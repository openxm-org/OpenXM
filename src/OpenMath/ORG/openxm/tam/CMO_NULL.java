/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の NULL 型を表します.
 */
final public class CMO_NULL extends CMO{
  /**
   * NULL を作成します.
   */
  public CMO_NULL(){}

  public int DISCRIMINATOR(){
    return CMO.NULL;
  }

  public void sendByObject(OpenXMconnection os) throws IOException{
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    return new CMO_NULL();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
