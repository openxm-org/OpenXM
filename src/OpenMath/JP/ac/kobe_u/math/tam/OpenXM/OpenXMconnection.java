/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OpenXMconnection.java,v 1.13 2000/03/15 15:02:07 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

class OpenXMconnection{
  protected int serial_num = 0x12345678;
  private Socket socket = null;
  private InputStream  is = null;
  private OutputStream os = null;
  private int order = OX_BYTE_NETWORK_BYTE_ORDER;
  private CMO_MATHCAP mathcap = null;
  private ByteArrayOutputStream buffer = new ByteArrayOutputStream();

  // OX message_tag
  final public static int OX_COMMAND                  = 513;
  final public static int OX_DATA                     = 514;
  final public static int OX_SYNC_BALL                = 515;
  final public static int OX_DATA_WITH_LENGTH         = 521;
  final public static int OX_DATA_OPENMATH_XML        = 523;
  final public static int OX_DATA_OPENMATH_BINARY     = 524;
  final public static int OX_DATA_MP                  = 525;
  final public static int OX_PRIVATE                  = 0x7fff0000;

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

  /*
  public void write(byte[] buf) throws IOException{
    for(int i=0;i<buf.length;i++){
      buffer.write(b[i]);
    }
  }
  */

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


  public void sendSM(OXbody code) throws IOException,MathcapViolation{
    this.write(new OXmessage(serial_num,code));
  }

  public void send(OXbody object) throws IOException,MathcapViolation{
    this.sendSM(object);
  }

  public void sendCMO(CMO object) throws IOException,MathcapViolation{
    ByteArrayOutputStream bos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(bos);

    if(mathcap != null){
      CMO[] list =((CMO_LIST)mathcap.getList().getElements()[2]).getElements();

      for(int i=0;i<list.length;i++){
	CMO[] datacap = ((CMO_LIST)list[i]).getElements();

	if(((CMO_INT32)datacap[0]).intValue() == OX_DATA){
	  CMO[] tagcap = ((CMO_LIST)datacap[1]).getElements();

	  CMO.mathcap = new int[tagcap.length];
	  for(int j=0;j<tagcap.length;j++){
	    CMO.mathcap[j] = ((CMO_INT32)tagcap[j]).intValue();
	  }
	  break;
	}
      }
    }

    this.write(new OXmessage(serial_num,object));
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

  public static String toString(int tag){
    switch(tag){
    case OX_COMMAND:
      return "OX_COMMAND";

    case OX_DATA:
      return "OX_DATA";

    case OX_SYNC_BALL:
      return "OX_SYNC_BALL";
    }

    return "";
  }
}
