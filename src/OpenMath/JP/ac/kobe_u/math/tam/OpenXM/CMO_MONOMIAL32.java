/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_MONOMIAL32.java,v 1.8 2000/03/15 17:58:07 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

final public class CMO_MONOMIAL32 extends CMO{
  private int[] degree;
  private CMO coefficient;

  public CMO_MONOMIAL32(int[] degree,CMO coefficient){
    this.degree = degree;
    this.coefficient = coefficient;
  }

  public int DISCRIMINATOR(){
    return CMO.MONOMIAL32;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    os.writeInt(degree.length);
    for(int i=0;i<degree.length;i++){
      os.writeInt(degree[i]);
    }
    coefficient.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
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

  public CMO getCoefficient(){
    return coefficient;
  }

  public int[] getDegree(){
    return degree;
  }
}
