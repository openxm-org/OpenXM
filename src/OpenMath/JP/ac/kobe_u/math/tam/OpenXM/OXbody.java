/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public abstract class OXbody{

  public OXbody(DataInputStream is) throws IOException{
  }

  public abstract void write(DataOutputStream os) throws IOException,MathcapViolation;

  //public abstract static OX read(DataInputStream is) throws IOException;

  public abstract String toOXexpression();

  public String toString(){
    return this.toOXexpression();
  }
}
