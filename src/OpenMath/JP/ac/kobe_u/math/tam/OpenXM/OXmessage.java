/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OXmessage.java,v 1.1 2000/02/02 11:48:32 tam Exp $
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

  public OXmessage(DataInputStream is) throws IOException{
    tag    = is.readInt();
    serial = is.readInt();
  }

  public OXmessage(OpenXMconnection is) throws IOException{
    tag    = is.readInt();
    serial = is.readInt();
  }

  public int getTag(){
    return this.tag;
  }

  public void write(DataOutputStream os) throws IOException,MathcapViolation{
    os.writeInt(this.tag);
    os.writeInt(this.serial);
    body.write(os);
    return;
  }

  public void write(OpenXMconnection os) throws IOException,MathcapViolation{
    os.writeInt(this.tag);
    os.writeInt(this.serial);
    body.write(os);
    return;
  }

  public static OX read(DataInputStream is) throws IOException{
    return new OX(is);
  }

  public String toOXexpression(){
    //return "("+ this.toCMOexpressionByObject() +")";
    return "";
  }

  public String toString(){
    return this.toOXexpression();
  }
}
