/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OXbody.java,v 1.1 2000/02/02 11:48:32 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public abstract class OXbody{

  //private protected OXbody(){}

  //public OXbody(DataInputStream is) throws IOException{}

  public abstract void write(DataOutputStream os) throws IOException,MathcapViolation;

  //public abstract static OX read(DataInputStream is) throws IOException;

  public abstract String toOXexpression();

  public String toString(){
    return this.toOXexpression();
  }
}
