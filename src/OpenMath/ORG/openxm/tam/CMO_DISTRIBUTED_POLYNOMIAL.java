/**
 * $OpenXM$
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の distributed polynomial 型を表します.
 */
final public class CMO_DISTRIBUTED_POLYNOMIAL extends CMO{
  private CMO ring;
  private CMO_MONOMIAL32[] monomials;

  /**
   * 環 ring 上の MONOMIAL32 型の配列 monomials を持つ多項式を作成します.
   */
  public CMO_DISTRIBUTED_POLYNOMIAL(CMO ring,CMO_MONOMIAL32[] monomials){
    this.ring = ring;
    this.monomials = monomials;
  }

  public int DISCRIMINATOR(){
    return CMO.DISTRIBUTED_POLYNOMIAL;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    if(monomials.length == 0){
      os.writeInt(1);
      ring.write(os);
      new CMO_ZERO().write(os);
    }else{
      os.writeInt(monomials.length);
      ring.write(os);
      for(int i=0;i<monomials.length;i++){
	monomials[i].write(os);
      }
    }
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    CMO ring;
    CMO_MONOMIAL32[] monomials;
    CMO tmp;
    int m;

    m = is.readInt();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_DMS_GENERIC)){
      System.err.println("not CMOobject");
    }
    ring = tmp;

    tmp = (CMO_MONOMIAL32)CMO.receive(is);
    if(tmp instanceof CMO_ZERO && m == 1){
      monomials = new CMO_MONOMIAL32[0];
    }else{
      monomials = new CMO_MONOMIAL32[m];
      monomials[0] = (CMO_MONOMIAL32)tmp;
    }

    for(int i=1;i<m;i++){
      monomials[i] = (CMO_MONOMIAL32)CMO.receive(is);
    }

    return new CMO_DISTRIBUTED_POLYNOMIAL(ring,monomials);
  }

  public String toCMOexpressionByObject(){
    String ret = "CMO_DISTRIBUTED_POLYNOMIAL,";

    if(monomials.length == 0){
      ret += 1 +",";
      ret += ring.toCMOexpression();
      ret += ","+ new CMO_ZERO().toCMOexpression();
    }else{
      ret += monomials.length +",";
      ret += ring.toCMOexpression();
      for(int i=0;i<monomials.length;i++){
	ret += ","+ monomials[i].toCMOexpression();
      }
    }

    return ret;
  }

  /**
   * 多項式が定義されている環を得ます.
   */
  public CMO getRing(){
    return ring;
  }

  /**
   * 多項式の各 monomial を得ます.
   */
  public CMO_MONOMIAL32[] getMonomials(){
    return monomials;
  }
}
