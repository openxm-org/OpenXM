/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_TREE.java,v 1.6 2000/03/14 05:38:51 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

final public class CMO_TREE extends CMO{
  private String name,cdname;
  private CMO_LIST leaves;

  public CMO_TREE(String name,String cdname,CMO_LIST leaves){
    this.name = name;
    this.cdname = cdname;
    this.leaves = leaves;
  }

  public String getName(){
    return name;
  }

  public String getCDName(){
    return cdname;
  }

  public CMO_LIST getLeaves(){
    return leaves;
  }

  public int DISCRIMINATOR(){
    return CMO.TREE;
  }

  public void sendByObject(OpenXMconnection os)
       throws IOException,MathcapViolation{
    new CMO_STRING(name).write(os);
    new CMO_STRING(cdname).write(os);
    leaves.write(os);
  }

  static protected CMO receive(OpenXMconnection is) throws IOException{
    String name,cdname;
    CMO_LIST leaves;
    CMO tmp;

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_STRING)){
      System.err.println("not CMOobject");
    }
    name = ((CMO_STRING)tmp).getString();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_STRING)){
      System.err.println("not CMOobject");
    }
    cdname = ((CMO_STRING)tmp).getString();

    tmp = CMO.receive(is);
    if(!(tmp instanceof CMO_LIST)){
      System.err.println("not CMOobject");
    }
    leaves = (CMO_LIST)tmp;

    return new CMO_TREE(name,cdname,leaves);
  }

  public String toCMOexpressionByObject(){
    return "CMO_TREE,"+ name +","+ cdname +","+ leaves.toCMOexpression();
  }
}
