/**
 * $OpenXM$
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

  public int DISCRIMINATOR(){
    return CMO_RECURSIVE_POLYNOMIAL;
  }

  public void sendByObject(DataOutputStream os) throws IOException{
    variables.send(os);
    polynomial.send(os);
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

  public String toCMOexpressionByObject(){
    String ret = "CMO_RECURSIVE_POLYNOMIAL";

    ret += ","+ variables.toCMOexpression();
    ret += ","+ polynomial.toCMOexpression();

    return ret;
  }

  public CMO_LIST getVariables(){
    return variables;
  }

  public CMO getPolynomial(){
    return polynomial;
  }
}
