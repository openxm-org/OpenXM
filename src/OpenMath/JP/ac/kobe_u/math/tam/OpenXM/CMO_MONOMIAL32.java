/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_MONOMIAL32.java,v 1.3 1999/11/14 22:57:45 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.math.BigInteger;

public class CMO_MONOMIAL32 extends CMO{
  private int[] degree;
  private CMO coefficient;

  public CMO_MONOMIAL32(){}

  public CMO_MONOMIAL32(int[] degree,CMO_ZZ coefficient){
    this.degree = degree;
    this.coefficient = coefficient;
  }

  public int DISCRIMINATOR(){
    return CMO_MONOMIAL32;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    os.writeInt(degree.length);
    for(int i=0;i<degree.length;i++){
      os.writeInt(degree[i]);
    }
    coefficient.send(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
    int n;

    n = is.readInt();
    degree = new int[n];

    for(int i=0;i<n;i++){
      degree[i] = is.readInt();
    }

    coefficient = receive(is);

    return this;
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
