/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_LIST.java,v 1.2 2000/09/13 06:32:42 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の LIST 型を表します.
 */
final public class CMO_LIST extends CMO{
  private CMO ob[];

  /**
   * CMO 配列 src を要素とする LIST を作成します.
   */
  public CMO_LIST(CMO[] src){
    ob = src;
  }

  /**
   * 要素を得ます.
   */
  public CMO[] getElements(){
    return ob;
  }

  public int DISCRIMINATOR(){
    return CMO.LIST;
  }

    public boolean allowQ (int[] datacap) {
		if(CMO.allowQ_tag(datacap, DISCRIMINATOR())) {
			for (int j=0; j < ob.length; j++) {
				if (!ob[j].allowQ(datacap)) {
					return false;
				}
			}
			return true;
        }
        return false;
    }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    os.writeInt(ob.length);

    for(int i=0;i<ob.length;i++){
      ob[i].write(os);
    }
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
    int len;
    CMO[] ob;

    len = is.readInt();
    ob = new CMO[len];

    for(int i=0;i<len;i++){
      ob[i] = CMO.receive(is);
    }

    return new CMO_LIST(ob);
  }

  public String toCMOexpressionByObject(){
    String str = "";

    for(int i=0;i<ob.length;i++){
      //System.err.print(":"+ ob[i]);
      str += ","+ ob[i].toCMOexpression();
    }

    return "CMO_LIST,"+ ob.length + str;
  }
}
