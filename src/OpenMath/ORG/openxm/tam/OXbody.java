/*
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * OpenXM メッセージのボディ部分を表す抽象クラス.
 */
public abstract class OXbody{

  //private protected OXbody(){}

  //public OXbody(DataInputStream is) throws IOException{}

  /**
   * os へオブジェクトを送信します.
   * このメソッドではヘッダ部分の送信を記述する必要はありません.
   */
  public abstract void write(OpenXMstream os) throws IOException,MathcapViolation;

  //public abstract static OX read(DataInputStream is) throws IOException;

  /**
   * オブジェクトを OXexpression に変換します.
   * このメソッドではヘッダ部分の変換を記述する必要はありません.
   */
  public abstract String toOXexpression();

  /**
   * オブジェクトを String 型に変換します.
   * 現在は OXexpression へ変換します.
   */
  public String toString(){
    return this.toOXexpression();
  }
}
