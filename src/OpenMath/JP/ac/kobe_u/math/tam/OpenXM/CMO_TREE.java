/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/CMO_TREE.java,v 1.4 2000/01/20 18:14:33 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class CMO_TREE extends CMO{
  private String name,cdname;
  private CMO_LIST leaves;

  public CMO_TREE(){}

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
    return CMO_TREE;
  }

  public void sendByObject(DataOutputStream os)
       throws IOException,MathcapViolation{
    new CMO_STRING(name).write(os);
    new CMO_STRING(cdname).write(os);
    leaves.write(os);
  }

  protected CMO receiveByObject(DataInputStream is) throws IOException{
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

    return this;
  }

  public String toCMOexpressionByObject(){
    return "CMO_TREE,"+ name +","+ cdname +","+ leaves.toCMOexpression();
  }
}
