/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OpenXMconnection.java,v 1.17 2000/03/16 09:51:32 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

class OpenXMconnection{
  private int serial = 0;
  private Socket socket = null;
  private InputStream  is = null;
  private OutputStream os = null;
  private int order = OX_BYTE_NETWORK_BYTE_ORDER;
  private CMO_MATHCAP mathcap = null;
  private ByteArrayOutputStream buffer = new ByteArrayOutputStream();

  // byte order (support network byte order only)
  final public static int OX_BYTE_NETWORK_BYTE_ORDER = 0;
  final public static int OX_BYTE_LITTLE_INDIAN      = 1;
  final public static int OX_BYTE_BIG_INDIAN         = 0xff;

  public OpenXMconnection(String host,int Port) throws IOException{
    this(host,Port,false);
  }

  public OpenXMconnection(String host,int Port,boolean reverse) throws IOException{
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

  public int sendByteOrder() throws IOException{
    // send byte order
    os.write(OX_BYTE_NETWORK_BYTE_ORDER);
    os.flush();

    // receive byte order
    //System.out.println("get: "+is.read());
    is.read();

    order = OX_BYTE_NETWORK_BYTE_ORDER;

    return order;
  }

  public void close() throws IOException{
    is.close();
    os.close();
    socket.close();
  }

  public void writeByte(int b) throws IOException{
    buffer.write(b);
  }

  public void writeInt(int i) throws IOException{
    new DataOutputStream(buffer).writeInt(i);
  }

  public byte readByte() throws IOException{
    return (byte)is.read();
  }

  public int readInt() throws IOException{
    return new DataInputStream(is).readInt();
  }

  public void write(OXmessage message) throws IOException,MathcapViolation{
    buffer.reset();
    message.write(this);
    buffer.writeTo(os);
    os.flush();
  }

  public void send(OXbody object) throws IOException,MathcapViolation{
    this.write(new OXmessage(serial++,object));
  }

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

  public void sendOX_SYNC_BALL() throws IOException,MathcapViolation{
    this.send(null);
  }

  public OXmessage receive() throws IOException{
    return new OXmessage(this);
  }

  public void setMathCap(CMO_MATHCAP mathcap){
    this.mathcap = mathcap;
  }
}
