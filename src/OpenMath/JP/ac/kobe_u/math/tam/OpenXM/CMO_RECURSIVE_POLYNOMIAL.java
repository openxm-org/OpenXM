/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_RECURSIVE_POLYNOMIAL.java,v 1.3 2000/01/20 18:14:33 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_RECURSIVE_POLYNOMIAL extends CMO{
  CMO_LIST variables;
  CMO polynomial;

  public CMO_RECURSIVE_POLYNOMIAL(){}

  public CMO_RECURSIVE_POLYNOMIAL(CMO_LIST variables,CMO polynomial){
    this.variables = variables;
    this.polynomial = polynomial;
  }

  public CMO_LIST getVariables(){
    return variables;
  }

  public CMO getPolynomial(){
    return polynomial;
  }

  public int DISCRIMINATOR(){
    return CMO_RECURSIVE_POLYNOMIAL;
  }

  protected void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    variables.write(os);
    polynomial.write(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_LIST)){
      System.err.println("not CMOobject");
    }
    variables = (CMO_LIST)tmp;

    polynomial = receive(is);

    return this;
  }

  protected String toCMOexpressionByObject(){
    String ret = "CMO_RECURSIVE_POLYNOMIAL";

    ret += ","+ variables.toCMOexpression();
    ret += ","+ polynomial.toCMOexpression();

    return ret;
  }
}
