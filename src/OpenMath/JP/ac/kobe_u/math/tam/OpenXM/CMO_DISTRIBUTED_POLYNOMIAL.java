/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_DISTRIBUTED_POLYNOMIAL.java,v 1.2 1999/11/07 21:22:03 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_DISTRIBUTED_POLYNOMIAL extends CMO{
  CMO ring;
  CMO_MONOMIAL32[] monomials;

  public CMO_DISTRIBUTED_POLYNOMIAL(){}

  public CMO_DISTRIBUTED_POLYNOMIAL(CMO ring,CMO_MONOMIAL32[] monomials){
    this.ring = ring;
    this.monomials = monomials;
  }

  public int DISCRIMINATOR(){
    return CMO_DISTRIBUTED_POLYNOMIAL;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    if(monomials.length == 0){
      os.writeInt(1);
      ring.send(os);
      new CMO_ZERO().send(os);
    }else{
      os.writeInt(monomials.length);
      ring.send(os);
      for(int i=0;i<monomials.length;i++){
	monomials[i].send(os);
      }
    }
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    CMO tmp;
    int m;

    m = is.readInt();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_DMS_GENERIC)){
      System.err.println("not CMOobject");
    }
    ring = tmp;

    tmp = (CMO_MONOMIAL32)receive(is);
    if(tmp instanceof CMO_ZERO && m == 1){
      monomials = new CMO_MONOMIAL32[0];
    }else{
      monomials = new CMO_MONOMIAL32[m];
      monomials[0] = (CMO_MONOMIAL32)tmp;
    }

    for(int i=1;i<m;i++){
      monomials[i] = (CMO_MONOMIAL32)receive(is);
    }

    return this;
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

  public CMO getRing(){
    return ring;
  }

  public CMO_MONOMIAL32[] getMonomials(){
    return monomials;
  }
}
