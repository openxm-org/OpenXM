/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の STRING 型を表します.
 */
final public class CMO_STRING extends CMO{
  private String str = null;

  /**
   * 空文字列を内容とする STRING を作成します.
   */
  public CMO_STRING(){
    str = null;
  }

  /**
   * 文字列 a を内容とする STRING を作成します.
   */
  public CMO_STRING(String a){
    str = a;
  }

  /**
   * 内容の文字列を得ます.
   */
  public String getString(){
    return str;
  }

  public int DISCRIMINATOR(){
    return CMO.STRING;
  }

  protected void sendByObject(OpenXMconnection os) throws IOException{
    byte[] buf = str.getBytes();

    os.writeInt(buf.length);
    for(int i=0;i<buf.length;i++){
      os.writeByte(buf[i]);
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    int len;
    byte[] buf=null;

    len=is.readInt();
    if(len==0){
      return new CMO_STRING("");
    }

    buf = new byte[len];
    for(int i=0;i<len;i++){
      buf[i] = is.readByte();
    }

    return new CMO_STRING(new String(buf));
  }

  protected String toCMOexpressionByObject(){
    return "CMO_STRING,"+ str.length() +","+ str;
  }
}
