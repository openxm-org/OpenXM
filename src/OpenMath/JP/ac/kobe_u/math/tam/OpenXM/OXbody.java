/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OXbody.java,v 1.2 2000/02/03 11:04:35 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public abstract class OXbody{

  //private protected OXbody(){}

  //public OXbody(DataInputStream is) throws IOException{}

  public abstract void write(OpenXMconnection os) throws IOException,MathcapViolation;

  //public abstract static OX read(DataInputStream is) throws IOException;

  public abstract String toOXexpression();

  public String toString(){
    return this.toOXexpression();
  }
}
