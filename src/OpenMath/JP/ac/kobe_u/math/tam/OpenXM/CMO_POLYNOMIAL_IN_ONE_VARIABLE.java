/**
 * $OpenXM$
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_POLYNOMIAL_IN_ONE_VARIABLE extends CMO{
  private int variable;
  private int[] degrees;
  private CMO[] coefficients;

  public CMO_POLYNOMIAL_IN_ONE_VARIABLE(int variable,int[] degrees,CMO[] coefficients){
    this.variable = variable;
    this.degrees = degrees;
    this.coefficients = coefficients;
  }

  public int getVariable(){
    return variable;
  }

  public int[] getDegrees(){
    return degrees;
  }

  public CMO[] getCoefficients(){
    return coefficients;
  }

  public int DISCRIMINATOR(){
    return POLYNOMIAL_IN_ONE_VARIABLE;
  }

  protected void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    int m = degrees.length;

    new CMO_INT32(m).write(os);
    new CMO_INT32(variable).write(os);
    for(int i=0;i<m;i++){
      new CMO_INT32(degrees[i]).write(os);
      coefficients[i].write(os);
    }
  }

  static protected CMO receive(DataInputStream is) throws IOException{
    int variable;
    int[] degrees;
    CMO[] coefficients;
    int m;

    m = is.readInt();
    variable = is.readInt();

    degrees = new int[m];
    coefficients = new CMO[m];
    for(int i=0;i<m;i++){
      degrees[i] = is.readInt();
      coefficients[i] = CMO.receive(is);
    }
 
    return new CMO_POLYNOMIAL_IN_ONE_VARIABLE(variable,degrees,coefficients);
  }

  protected String toCMOexpressionByObject(){
    String ret;

    ret = "CMO_POLYNOMIAL_IN_ONE_VARIABLE,"+ degrees.length +","+ variable;
    for(int i=0;i<degrees.length;i++){
      ret += ","+ degrees[i];
      ret += ","+ coefficients[i].toCMOexpression();
    }
    return ret;
  }
}
