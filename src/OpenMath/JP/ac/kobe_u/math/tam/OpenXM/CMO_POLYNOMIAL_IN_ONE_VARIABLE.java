/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_POLYNOMIAL_IN_ONE_VARIABLE.java,v 1.1 1999/11/16 00:18:30 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_POLYNOMIAL_IN_ONE_VARIABLE extends CMO{
  private int variable;
  private int[] degree;
  private CMO[] coefficient;

  public CMO_POLYNOMIAL_IN_ONE_VARIABLE(){}

  public CMO_POLYNOMIAL_IN_ONE_VARIABLE(int variable,int[] degree,CMO[] coefficient){
    this.variable = variable;
    this.degree = degree;
    this.coefficient = coefficient;
  }

  public int DISCRIMINATOR(){
    return CMO_POLYNOMIAL_IN_ONE_VARIABLE;
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int m;

    m = is.readInt();
    variable = is.readInt();

    degree = new int[m];
    coefficient = new CMO[m];
    for(int i=0;i<m;i++){
      degree[i] = is.readInt();
      coefficient[i] = CMO.receive(is);
    }
 
    return new CMO_POLYNOMIAL_IN_ONE_VARIABLE(variable,degree,coefficient);
  }

  protected void sendByObject(DataOutputStream os) throws IOException{
    int m = degree.length;

    new CMO_INT32(m).send(os);
    new CMO_INT32(variable).send(os);
    for(int i=0;i<m;i++){
      new CMO_INT32(degree[i]).send(os);
      coefficient[i].send(os);
    }
  }

  public String toCMOexpressionByObject(){
    String ret;

    ret = "CMO_POLYNOMIAL_IN_ONE_VARIABLE,"+ degree.length +","+ variable;
    for(int i=0;i<degree.length;i++){
      ret += ","+ degree[i];
      ret += ","+ coefficient[i].toCMOexpression();
    }
    return ret;
  }
}
