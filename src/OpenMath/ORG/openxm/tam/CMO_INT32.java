/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_INT32.java,v 1.1 2000/09/12 07:05:06 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の INT32 型を表します.
 */
final public class CMO_INT32 extends CMO{
  private int value;

  /**
   * i を値とする INT32 を作成します.
   */
  public CMO_INT32(int i){
    value = i;
  }

  public int DISCRIMINATOR(){
    return CMO.INT32;
  }

  /**
   * 値を得ます.
   */
  public int intValue(){
    return value;
  }

  public void sendByObject(OpenXMstream os) throws IOException{
    os.writeInt(value);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int value = is.readInt();

    return new CMO_INT32(value);
  }

  public String toCMOexpressionByObject(){
    return "CMO_INT32,"+ value;
  }
}
