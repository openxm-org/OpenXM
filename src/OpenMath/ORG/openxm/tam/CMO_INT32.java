/**
 * $OpenXM$
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

  public void sendByObject(OpenXMconnection os) throws IOException{
    os.writeInt(value);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int value = is.readInt();

    return new CMO_INT32(value);
  }

  public String toCMOexpressionByObject(){
    return "CMO_INT32,"+ value;
  }
}
