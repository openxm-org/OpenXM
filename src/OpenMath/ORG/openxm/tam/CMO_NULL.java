/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_NULL.java,v 1.1 2000/09/12 07:05:06 tam Exp $
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

  public void sendByObject(OpenXMstream os) throws IOException{
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    return new CMO_NULL();
  }

  public String toCMOexpressionByObject(){
    return "CMO_NULL";
  }
}
