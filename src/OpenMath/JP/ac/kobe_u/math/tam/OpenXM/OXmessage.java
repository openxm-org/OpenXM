/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OXmessage.java,v 1.4 2000/03/16 09:51:32 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class OXmessage{
  OXbody body = null;
  int tag;
  int serial;

  // OX message_tag
  final public static int OX_COMMAND                  = 513;
  final public static int OX_DATA                     = 514;
  final public static int OX_SYNC_BALL                = 515;
  final public static int OX_DATA_WITH_LENGTH         = 521;
  final public static int OX_DATA_OPENMATH_XML        = 523;
  final public static int OX_DATA_OPENMATH_BINARY     = 524;
  final public static int OX_DATA_MP                  = 525;
  final public static int OX_PRIVATE                  = 0x7fff0000;

  public OXmessage(OpenXMconnection is) throws IOException{
    tag    = is.readInt();
    serial = is.readInt();
    switch(tag){
    case OX_COMMAND:
      body = SM.receive(is);
      break;

    case OX_DATA:
      body = CMO.receive(is);
      break;

    case OX_SYNC_BALL:
      body = null;
      break;
    }
  }

  public OXmessage(int serial,OXbody body){
    this.serial = serial;
    this.body = body;

    if(body == null){
      this.tag = OX_SYNC_BALL;
    }else if(body instanceof SM){
      this.tag = OX_COMMAND;
    }else if(body instanceof CMO){
      this.tag = OX_DATA;
    }else{
      this.tag = OX_PRIVATE;
    }
  }

  public int getTag(){
    return this.tag;
  }

  public OXbody getBody(){
    return this.body;
  }

  public void write(OpenXMconnection os) throws IOException,MathcapViolation{
    os.writeInt(this.tag);
    os.writeInt(this.serial);
    body.write(os);
    return;
  }

  public String toOXexpression(){
    //return "("+ this.toCMOexpressionByObject() +")";
    return "";
  }

  public String toString(){
    return this.toOXexpression();
  }
}
