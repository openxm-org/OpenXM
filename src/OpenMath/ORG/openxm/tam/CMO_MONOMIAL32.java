/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_MONOMIAL32.java,v 1.2 2000/09/13 06:32:42 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.math.BigInteger;

/**
 * CMO 形式の MONOMIAL32 型を表します.
 * このクラスは単項式を表現します.
 */
final public class CMO_MONOMIAL32 extends CMO{
  private int[] degree;
  private CMO coefficient;

  /**
   * 係数 coefficient, 次数 degree[] とする MONOMIAL32 を作成します.
   */
  public CMO_MONOMIAL32(int[] degree,CMO coefficient){
    this.degree = degree;
    this.coefficient = coefficient;
  }

  public int DISCRIMINATOR(){
    return CMO.MONOMIAL32;
  }

    public boolean allowQ (int[] datacap) {
		return CMO.allowQ_tag(datacap, DISCRIMINATOR()) 
			&& coefficient.allowQ(datacap);
    }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    os.writeInt(degree.length);
    for(int i=0;i<degree.length;i++){
      os.writeInt(degree[i]);
    }
    coefficient.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int n;
    int[] degree;
    CMO coefficient;

    n = is.readInt();
    degree = new int[n];

    for(int i=0;i<n;i++){
      degree[i] = is.readInt();
    }

    coefficient = CMO.receive(is);

    return new CMO_MONOMIAL32(degree,coefficient);
  }

  public String toCMOexpressionByObject(){
    String ret;

    ret = "CMO_MONOMIAL32,"+ degree.length;

    for(int i=0;i<degree.length;i++){
      ret += ","+ degree[i] ;
    }

    return ret +","+ coefficient.toCMOexpression();
  }

  /**
   * 係数を得ます.
   */
  public CMO getCoefficient(){
    return coefficient;
  }

  /**
   * 次数を得ます.
   */
  public int[] getDegree(){
    return degree;
  }
}
