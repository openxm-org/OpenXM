/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OpenXMconnection.java,v 1.8 2000/01/19 15:21:58 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

class OpenXMconnection{
  protected int serial_num = 0x12345678;
  Socket socket = null;
  InputStream  is = null;
  OutputStream os = null;
  int order = OX_BYTE_NETWORK_BYTE_ORDER;
  CMO_MATHCAP mathcap = null;

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

  public void sendSM(SM code) throws IOException{
    DataOutputStream dos = new DataOutputStream(os);

    dos.writeInt(OX_COMMAND);
    dos.writeInt(serial_num);
    code.send(dos);
    os.flush();
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

    dos.writeInt(OX_DATA);
    dos.writeInt(serial_num);
    object.send(dos);
    dos.flush();
    bos.writeTo(os);
  }

  public void send(Object object) throws IOException,MathcapViolation{
    if(object instanceof CMO){
      sendCMO((CMO)object);
    }else if(object instanceof SM){
      sendSM((SM)object);
    }
  }

  public void sendOX_SYNC_BALL() throws IOException{
    DataOutputStream dos = new DataOutputStream(os);

    dos.writeInt(OX_SYNC_BALL);
    dos.writeInt(serial_num);
  }

  public CMO receiveCMO() throws IOException{
    return CMO.receive(new DataInputStream(is));
  }

  public SM receiveSM() throws IOException{
    return new SM(is);
  }

  public int receiveOXtag() throws IOException{
    DataInputStream dis = new DataInputStream(is);
    int tag = dis.readInt();

    dis.readInt(); // read serial

    return tag;
  }

  public void setMathcap(CMO_MATHCAP mathcap){
    this.mathcap = mathcap;
  }

  public void receive() throws IOException{
    DataInputStream dis = new DataInputStream(is);
    int tag = receiveOXtag();

    System.out.print("("+ toString(tag) +",");
    //System.out.print("serial="+ dis.readInt() +"," );

    switch(tag){
    case OX_COMMAND:
      System.out.print(""+ receiveSM());
      break;

    case OX_DATA:
      System.out.print(""+ receiveCMO());
      break;
    }

    System.out.println(")");
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
