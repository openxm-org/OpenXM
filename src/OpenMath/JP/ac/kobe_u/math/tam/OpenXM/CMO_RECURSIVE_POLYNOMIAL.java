/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_RECURSIVE_POLYNOMIAL.java,v 1.6 2000/03/15 17:58:07 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_RECURSIVE_POLYNOMIAL extends CMO{
  private CMO_LIST variables;
  private CMO polynomial;

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
    return CMO.RECURSIVE_POLYNOMIAL;
  }

  protected void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    variables.write(os);
    polynomial.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO_LIST variables;
    CMO polynomial;
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_LIST)){
      System.err.println("not CMOobject");
    }
    variables = (CMO_LIST)tmp;

    polynomial = CMO.receive(is);

    return new CMO_RECURSIVE_POLYNOMIAL(variables,polynomial);
  }

  protected String toCMOexpressionByObject(){
    String ret = "CMO_RECURSIVE_POLYNOMIAL";

    ret += ","+ variables.toCMOexpression();
    ret += ","+ polynomial.toCMOexpression();

    return ret;
  }
}
