/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の RECURSIVE POLYNOMIAL 型を表します.
 */
final public class CMO_RECURSIVE_POLYNOMIAL extends CMO{
  private CMO_LIST variables;
  private CMO polynomial;

  /**
   * 変数 variables の多項式 polynomial を作成します.
   * セマンティックスのチェックは行なわれません.
   */
  public CMO_RECURSIVE_POLYNOMIAL(CMO_LIST variables,CMO polynomial){
    this.variables = variables;
    this.polynomial = polynomial;
  }

  /**
   * 変数のリストを得ます.
   */
  public CMO_LIST getVariables(){
    return variables;
  }

  /**
   * 多項式を得ます.
   */
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
