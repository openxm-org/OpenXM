/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/OpenXM.java,v 1.5 2000/10/11 08:32:15 ohara Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;


/**
 * OpenXM サーバとの接続を行なうクラス.
 * クライアント側が使用する.
 * 接続するサーバ毎に一つの OpenXM クラスが必要.
 */
public class OpenXM{
  private OpenXMstream control = null, stream = null;
  final protected boolean debug = false;

  /**
   * OpenXM サーバとの接続を TCP/IP ソケットを用いて行なう.
   * マシン名 host のポート番号 CtrlPort にコントロールを,
   * ポート番号 StreamPort にデータ用の接続を行なう.
   */
  public OpenXM(String host,int CtrlPort,int StreamPort) throws IOException{
    control = new OpenXMstream(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMstream(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  /**
   * コマンド command を立ち上げ、
   * OpenXM サーバとの接続を TCP/IP ソケットを用いて行なう.
   * マシン名 host のポート番号 CtrlPort にコントロールを,
   * ポート番号 StreamPort にデータ用の接続を行なう.
   */
  public OpenXM(String command,String host,int CtrlPort,int StreamPort)
       throws IOException{
    Runtime.getRuntime().exec(command);
    control = new OpenXMstream(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMstream(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  /**
   * 接続の初期化を行なう. 現在は未実装.
   */
  public synchronized void resetConnection(){
    debug("control: stopping computer process...");
    debug("control: sending SYNC BALL.");
  }

  /**
   * OpenXM メッセージをデータストリームに送信する.
   * このメソッドはメッセージのボディの部分だけでよい.
   * ヘッダ部分は自動で付加される.
   */
  public void send(OXbody object) throws IOException,MathcapViolation{
    stream.send(object);
  }

  /**
   * データストリームから OpenXM メッセージを受け取る.
   */
  public OXmessage receive() throws IOException{
    return stream.receive();
  }

  /**
   * データストリームの MathCap を mathcap に設定する.
   * 以後, 送信するオブジェクトは mathcap に合っているかどうか
   * チェックが入る. 実際にチェックが入るかどうかは
   * OXbody クラスの派生クラスの実装による.
   * mathcap に反したオブジェクトを送ろうとした時には,
   * 以後 MathcapViolation が発生することが期待される.
   */
  public void setMathCap(CMO_MATHCAP mathcap){
    stream.setMathCap(mathcap);
  }

  private final void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  public static void main(String[] argv){
    String hostname = "localhost";
    int ControlPort = 1200, DataPort = 1300;
    Runnable process = null;
    Thread thread;
    OpenXM ox;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
        System.err.println("");
        System.exit(0);
      }else if(argv[i].equals("-host")){
        hostname = argv[++i];
      }else if(argv[i].equals("-data")){
        DataPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
        ControlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
        System.err.println("unknown option :"+ argv[i]);
        System.exit(1);
      }
    }

    try{
      ox = new OpenXM(hostname,ControlPort,DataPort);

      thread = new Thread(process);
      thread.start();
    }catch(UnknownHostException e){
      System.err.println("host unknown.");
      System.err.println(e.getMessage());
      return;
    }catch(IOException e){
      System.err.println("connection failed.");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }

    try{
      //サーバ側へ文字列を送信します。
      ox.send(new SM(SM.SM_mathcap));
      ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_STRING("print(\"Hello world!!\");\n"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));

      //ox.send(new CMO_STRING("def sub(B,A){return B-A;}"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));
      //ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_STRING("diff((x+2*y)^2,x);\0 1+2;"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));
      //ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_ZZ("1"));
      ox.send(new CMO_ZZ("-2"));
      //ox.send(new CMO_INT32(2));
      //ox.send(new CMO_STRING("sub"));
      //ox.send(new SM(SM.SM_executeFunction));
      ox.send(new SM(SM.SM_popCMO));

      { int[] array = {1,2};
      ox.send(new CMO_MONOMIAL32(array,new CMO_ZZ("-2")));
      }
      ox.send(new SM(SM.SM_popCMO));

      ox.send(new SM(SM.SM_popString));
      //ox.send(new SM(SM.SM_popString));

      ox.send(new SM(SM.SM_control_kill));

      //os.flush();

      //サーバ側から送信された文字列を受信します。
      while(true){
	ox.receive();
      }

    }catch(Exception e){
      e.printStackTrace();
    }

    System.err.println("breaking...");

    try{
      // close connection
      ox.stream.close();
      ox.control.close();
    }catch(IOException e){
      e.printStackTrace();
    }
  }
}
