/**
 * $OpenXM$
 */
package ORG.openxm.tam;

/**
 * OpenXM サーバの計算プロセスを記述するクラス.
 */
public abstract class EngineProcess{
  private OpenXMstream stream;

  public EngineProcess(OpenXMstream stream){
    this.stream = stream;

    new Thread(){
      public void run(){
	readEvalLoop(getOpenXMstream());
      }
    }.start();
  }

  private OpenXMstream getOpenXMstream(){
    return stream;
  }

  abstract private void readEvalLoop(OpenXMstream DataStream);

  /**
   * main メソッドの記述例
   final public static void main(String[] argv) throws java.io.IOException{
     new EngineProcess(new OpenXMstream(System.in,System.out));
   }
   */
}
