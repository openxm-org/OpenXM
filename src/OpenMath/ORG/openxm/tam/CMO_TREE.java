/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/CMO_TREE.java,v 1.1 2000/09/12 07:05:07 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;

/**
 * CMO 形式の TREE 型を表します.
 * これは OpenMath の Content Dictionary で定義されているオブジェクトを
 * 表すクラスです.
 */
final public class CMO_TREE extends CMO{
  private String name,cdname;
  private CMO_LIST leaves;

  /**
   * leaves を要素とし cdname で定義されている name を表す TREE を作成します.
   * セマンティックスはチェックされません.
   */
  public CMO_TREE(String name,String cdname,CMO_LIST leaves){
    this.name = name;
    this.cdname = cdname;
    this.leaves = leaves;
  }

  /**
   * 名前を得ます.
   */
  public String getName(){
    return name;
  }

  /**
   * 名前が定義されている CD の名前を得ます.
   */
  public String getCDName(){
    return cdname;
  }

  /**
   * 要素を得ます.
   */
  public CMO_LIST getLeaves(){
    return leaves;
  }

  public int DISCRIMINATOR(){
    return CMO.TREE;
  }

  public void sendByObject(OpenXMstream os)
       throws IOException,MathcapViolation{
    new CMO_STRING(name).write(os);
    new CMO_STRING(cdname).write(os);
    leaves.write(os);
  }

  static protected CMO receive(OpenXMstream is) throws IOException{
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
