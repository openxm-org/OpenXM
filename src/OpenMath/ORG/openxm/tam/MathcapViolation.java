/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * OpenXM メッセージを送信する際に発生する例外 Mathcap Violation を表します.
 * この例外は設定した mathcap に反したメッセージを送信しようとした
 * 場合に throw されます.
 */
public class MathcapViolation extends Exception{
  public MathcapViolation(){}

  public MathcapViolation(String s){
    super(s);
  }
}
