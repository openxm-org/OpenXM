/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_DMS_GENERIC.java,v 1.1 2000/09/12 07:05:05 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の DMS_GENERIC 型を表します.
 */
final public class CMO_DMS_GENERIC extends CMO{
  /**
   * DMS_GENERIC を作成します.
   */
  public CMO_DMS_GENERIC(){}

  public int DISCRIMINATOR(){
    return CMO.DMS_GENERIC;
  }

  public void sendByObject(OpenXMstream os) throws IOException{
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    return new CMO_DMS_GENERIC();
  }

  public String toCMOexpressionByObject(){
    return "CMO_DMS_GENERIC";
  }
}
