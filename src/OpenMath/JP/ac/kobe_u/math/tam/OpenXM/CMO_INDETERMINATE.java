/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_INDETERMINATE.java,v 1.2 1999/11/07 21:22:03 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_INDETERMINATE extends CMO{
  private CMO_STRING variable;

  public CMO_INDETERMINATE(){}

  public CMO_INDETERMINATE(String str){
    variable = new CMO_STRING(str);
  }

  public String getString(){
    return variable.getString();
  }

  public int DISCRIMINATOR(){
    return CMO_INDETERMINATE;
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_STRING)){
      System.err.println("not CMOobject");
    }
    variable = (CMO_STRING)tmp;

    return new CMO_INDETERMINATE(variable.getString());
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    variable.send(os);
  }

  public String toCMOexpressionByObject(){
    return "CMO_INDETERMINATE,"+ variable.toCMOexpression();
  }
}
