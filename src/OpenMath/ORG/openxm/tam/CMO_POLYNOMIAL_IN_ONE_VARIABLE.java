/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_POLYNOMIAL_IN_ONE_VARIABLE.java,v 1.2 2000/09/13 06:32:43 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の POLYNOMIAL IN ONE VARIABLE 型を表します.
 */
final public class CMO_POLYNOMIAL_IN_ONE_VARIABLE extends CMO{
  private int variable;
  private int[] degrees;
  private CMO[] coefficients;

  /**
   * 係数 coefficients[], 次数 degrees[], variable 番目の変数の多項式を表します.
   * セマンティックスのチェックは行なわれません.
   */
  public CMO_POLYNOMIAL_IN_ONE_VARIABLE(int variable,int[] degrees,CMO[] coefficients){
    this.variable = variable;
    this.degrees = degrees;
    this.coefficients = coefficients;
  }

  /**
   * 何番目の変数なのかを得ます.
   */
  public int getVariable(){
    return variable;
  }

  /**
   * 次数を表す配列を得ます.
   */
  public int[] getDegrees(){
    return degrees;
  }

  /**
   * 係数を表す配列を得ます.
   */
  public CMO[] getCoefficients(){
    return coefficients;
  }

  public int DISCRIMINATOR(){
    return CMO.POLYNOMIAL_IN_ONE_VARIABLE;
  }

    public boolean allowQ (int[] datacap) {
		if(CMO.allowQ_tag(datacap, DISCRIMINATOR())) {
			for (int j=0; j < coefficients.length; j++) {
				if (!coefficients[j].allowQ(datacap)) {
					return false;
				}
			}
			return true;
        }
        return false;
    }

  protected void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    int m = degrees.length;

    new CMO_INT32(m).write(os);
    new CMO_INT32(variable).write(os);
    for(int i=0;i<m;i++){
      new CMO_INT32(degrees[i]).write(os);
      coefficients[i].write(os);
    }
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
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
