/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_DMS.java,v 1.1 2000/09/12 07:05:05 tam Exp $
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

  public void sendByObject(OpenXMstream os) throws IOException{
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    return new CMO_DMS();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS";
  }
}
