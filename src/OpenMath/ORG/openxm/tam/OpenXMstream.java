/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/OpenXMstream.java,v 1.1 2000/09/08 07:29:45 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;

/**
 * OpenXM メッセージの送受信を行なうクラス.
 * コントロール, データそれぞれに一つずつ必要となる.
 * このクラスでは, 接続時にバイトオーダの交換を行ない,
 * 32bit 整数の送受信時にバイトオーダの変換を行なう.
 * 現在はネットワークバイトオーダのみ実装されている.
 */
public class OpenXMstream{
  private int serial = 0;
  private Socket socket = null;
  private InputStream  is = null;
  private OutputStream os = null;
  private int order = OX_BYTE_NETWORK_BYTE_ORDER;
  private CMO_MATHCAP mathcap = null;
  private ByteArrayOutputStream buffer = new ByteArrayOutputStream();

  /**
   * ネットワークバイトオーダを表す.
   * バイトオーダの決定時に用いる.
   */
  final public static int OX_BYTE_NETWORK_BYTE_ORDER = 0;

  /**
   * リトルインディアンを表す.
   * バイトオーダの決定時に用いる.
   */
  final public static int OX_BYTE_LITTLE_INDIAN      = 1;

  /**
   * ビッグインディアンを表す.
   * バイトオーダの決定時に用いる.
   */
  final public static int OX_BYTE_BIG_INDIAN         = 0xff;

  /**
   * マシン名 host のポート番号 Port へ TCP 接続を行ない,
   * 接続を行なう.
   */
  public OpenXMstream(String host,int Port) throws IOException{
    this(host,Port,false);
  }

  /**
   * マシン名 host のポート番号 Port へ TCP 接続を行ない,
   * 接続を行なう. reverse が true のとき、 TCP ソケットを
   * 作成し, 接続を待つ.
   */
  public OpenXMstream(String host,int Port,boolean reverse) throws IOException{
    // create socket
    if(!reverse){
      socket = new Socket(host,Port);
    }else{
      socket = new ServerSocket(Port,50,InetAddress.getByName(host)).accept();
    }

    //is =new DebugInputStream(new BufferedInputStream(socket.getInputStream()));
    is = new BufferedInputStream(socket.getInputStream());
    os = socket.getOutputStream();
  }

  /**
   * バイトオーダの決定を行なう. 現在はネットワークバイトオーダーのみ.
   */
  public int sendByteOrder() throws IOException{
    // send byte order
    os.write(OX_BYTE_NETWORK_BYTE_ORDER);
    os.flush();

    // receive byte order
    //System.err.println("get: "+is.read());
    is.read();

    order = OX_BYTE_NETWORK_BYTE_ORDER;

    return order;
  }

  /**
   * 接続を閉じる.
   */
  public void close() throws IOException{
    is.close();
    os.close();
    socket.close();
  }

  /**
   * 1 バイトの送信を行なう.
   */
  public void writeByte(int b) throws IOException{
    buffer.write(b);
  }

  /**
   * 32 bit integer の送信を行なう.
   * このメソッドは, バイトオーダの変換を自動で行なう.
   */
  public void writeInt(int i) throws IOException{
    new DataOutputStream(buffer).writeInt(i);
  }

  /**
   * 1 バイトの受信を行なう.
   */
  public byte readByte() throws IOException{
    return (byte)is.read();
  }

  /**
   * 32 bit integer の受信を行なう.
   * このメソッドは, バイトオーダの変換を自動で行なう.
   */
  public int readInt() throws IOException{
    return new DataInputStream(is).readInt();
  }

  /**
   * OpenXM メッセージの送信を行なう.
   */
  public void write(OXmessage message) throws IOException,MathcapViolation{
    buffer.reset();
    message.write(this);
    buffer.writeTo(os);
    os.flush();
  }

  /**
   * OpenXM メッセージの送信を行なう.
   * ボディのみでよく, ヘッダは自動で付加される.
   */
  public void send(OXbody object) throws IOException,MathcapViolation{
    this.write(new OXmessage(serial++,object));
  }

  /**
   * OXtag に相当する mathcap を得る.
   * mathcap が設定されていなければ, null が返される.
   */
  public CMO[] getMathcap(int OXtag) throws IOException,MathcapViolation{
    if(mathcap != null){
      CMO[] list =((CMO_LIST)mathcap.getList().getElements()[2]).getElements();

      for(int i=0;i<list.length;i++){
	CMO[] datacap = ((CMO_LIST)list[i]).getElements();

	if(((CMO_INT32)datacap[0]).intValue() == OXtag){
	  return datacap;
	}
      }
    }

    return null;
  }

  /**
   * OX_SYNC_BALL を送信する. 現在は未実装.
   */
  public void sendOX_SYNC_BALL() throws IOException,MathcapViolation{
    this.send(null);
  }

  /**
   * OpenXM メッセージを受信する.
   */
  public OXmessage receive() throws IOException{
    return new OXmessage(this);
  }

  /**
   * mathcap を設定する.
   * 以後, 送信する際に mathcap に適合しているかどうかチェックが行なわれる.
   */
  public void setMathCap(CMO_MATHCAP mathcap){
    this.mathcap = mathcap;
  }
}
