/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/DebugInputStream.java,v 1.1 2000/01/19 12:59:28 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;

public class DebugInputStream extends InputStream{
  private InputStream is = null;

  public DebugInputStream(InputStream is){
    this.is = is;
  }

  public int read() throws IOException{
    int a;

    a = is.read();
    System.out.print(" ("+a+")");
    return a;
  }
}
