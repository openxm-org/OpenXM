/**
 * $OpenXM: OpenXM/src/OpenMath/OXplot.java,v 1.3 2000/07/10 06:48:06 tam Exp $
 */

import JP.ac.kobe_u.math.tam.OpenXM.*;
import java.util.Stack;
import java.util.Vector;
import java.awt.*;

public class OXplot extends OpenXMserver{
  private Stack stack = new Stack();
  private Vector plotframe = new Vector();
  protected boolean debug = true;
  final int version = 200007010;

  public OXplot(String hostname,int ControlPort,int DataPort){
    super(hostname,ControlPort,DataPort);
  }

  public void computeProcess(OpenXMconnection stream){
    debug("OXplot started.");
    stack = new Stack();
    plotframe = new Vector();
    try{
      while(true){
	Thread.yield();
	try{
	  OXmessage message = stream.receive();
	  int ox_tag = message.getTag();

	  switch(ox_tag){
	  case OXmessage.OX_COMMAND:
	    StackMachine((SM)message.getBody(),stream);
	    break;

	  case OXmessage.OX_DATA:
	    stack.push(message.getBody());
	    debug("push: "+ stack.peek());
	    break;
	  }
	}catch(RuntimeException e){
	  System.err.println(e.getMessage());
	  e.printStackTrace();
	  debug("error occured. stack was cleared.");
	  stack = new Stack();
	}
      }
    }catch(java.io.IOException e){
      System.err.println(e.getMessage());
      e.printStackTrace();
      System.err.println("error occured, and recovering processes seems to be impossible.");
    }catch(Exception e){
      System.err.println(e.getMessage());
      e.printStackTrace();
      System.err.println("error occured, and recovering processes seems to be impossible.");
    }finally{
      System.out.println("breaking...");
    }
  }

  class plotframe extends java.awt.Frame implements java.awt.event.MouseListener{
    Canvas canvas;
    int pixels[][];

    plotframe(int width,int height){
      super("plotframe");
      add("Center", new Panel().add(canvas = new Canvas()));
      canvas.setSize(width,height);
      setResizable(false);
      canvas.addMouseListener(this);

      pixels = new int[height][];
      for(int i=0;i<pixels.length;i++){
	pixels[i] = new int[width];
	for(int j=0;j<pixels[i].length;j++){
	  pixels[i][j] = 255*j/width;
	}
      }

      pack();
      show();
    }

    public void paint(Graphics gr){
      paint();
    }

    public void paint(){
      Graphics g = canvas.getGraphics();

      for(int y=0;y<pixels.length;y++){
	for(int x=0;x<pixels[y].length;x++){
	  g.setColor(new Color(pixels[y][x],pixels[y][x],pixels[y][x]));
	  g.fillRect(x,y,1,1);
	}
      }
    }

    public void mouseClicked(java.awt.event.MouseEvent e){
      paint();
    }

    public void mousePressed(java.awt.event.MouseEvent e){
    }

    public void mouseReleased(java.awt.event.MouseEvent e){
    }

    public void mouseEntered(java.awt.event.MouseEvent e){
    }

    public void mouseExited(java.awt.event.MouseEvent e){
    }

    public void pset(int x,int y,int bright){
      pixels[y][x] = bright;
    }
  }

  private void SM_popCMO(OpenXMconnection stream) throws java.io.IOException{
    try{
      if(stack.empty()){
	stream.send(new CMO_NULL());
      }else{
	debug("sending CMO: "+ stack.peek());
	stream.send((CMO)stack.pop());
	debug("test");
      }
    }catch(MathcapViolation e){
      try{
	stream.send(new CMO_ERROR2(new CMO_STRING("MathcapViolation: "+
					      e.getMessage())));
      }catch(MathcapViolation tmp){}
    }
  }

  private void SM_executeFunction() throws java.io.IOException{
    String function_name;
    CMO[] argv;
    int argc;

    if(!(stack.peek() instanceof CMO_STRING)){
      stack.push(new CMO_ERROR2(new CMO_NULL()));
      return;
    }
    function_name = ((CMO_STRING)stack.pop()).getString();
    argc = ((CMO_INT32)stack.pop()).intValue();
    argv = new CMO[argc];
    for(int i=0;i<argc;i++){
      argv[i] = (CMO)stack.pop();
    }

    if(function_name.equals("create") && argc==2){
      stack.push(CREATE(argv));
    }else if(function_name.equals("pset") && argc==4){
      PSET(argv);
    }else{
      stack.push(new CMO_ERROR2(new CMO_NULL()));
    }

    return;
  }

  private void SM_mathcap(OpenXMconnection stream) throws java.io.IOException{
    CMO[] mathcap = new CMO[3];

    {
      CMO[] list = {new CMO_INT32(version),
		  new CMO_STRING("Ox_system=OMproxy.class"),
		  new CMO_STRING("Version=0."+ version),
		  new CMO_STRING("HOSTTYPE=JAVA")};
      mathcap[0] = new CMO_LIST(list);
    }

    {
      CMO[] list = {new CMO_INT32(SM.SM_popCMO),
		    new CMO_INT32(SM.SM_executeFunction),
		    new CMO_INT32(SM.SM_mathcap),
		    new CMO_INT32(SM.SM_setMathCap),
		    new CMO_INT32(SM.SM_control_kill),
		    new CMO_INT32(SM.SM_control_reset_connection)};
      mathcap[1] = new CMO_LIST(list);
    }

    {
      CMO[] CMOFormat = {new CMO_INT32(CMO.NULL),
			 new CMO_INT32(CMO.INT32),
			 new CMO_INT32(CMO.STRING),
			 new CMO_INT32(CMO.LIST),
			 new CMO_INT32(CMO.MONOMIAL32),
			 new CMO_INT32(CMO.ZZ),
			 new CMO_INT32(CMO.QQ),
			 new CMO_INT32(CMO.ZERO),
			 new CMO_INT32(CMO.BIGFLOAT),
			 new CMO_INT32(CMO.INDETERMINATE),
			 new CMO_INT32(CMO.TREE)};
      CMO[] DataFormat1 = {new CMO_INT32(OXmessage.OX_DATA),
			   new CMO_LIST(CMOFormat)};
      CMO[] list = {new CMO_LIST(DataFormat1)};

      mathcap[2] = new CMO_LIST(list);
    }

    stack = new Stack();
    stack.push(new CMO_MATHCAP(new CMO_LIST(mathcap)));
    debug("push: "+ stack.peek());
  }

  private void SM_setMathCap(OpenXMconnection stream)
       throws java.io.IOException{
    Object mathcap = stack.pop();

    if(mathcap instanceof CMO_MATHCAP){
      stack.push(new CMO_ERROR2(new CMO_NULL()));
    }
    stream.setMathCap((CMO_MATHCAP)mathcap);
  }

  private void StackMachine(SM mesg,OpenXMconnection stream)
       throws java.io.IOException{
    debug("receive: "+mesg);

    switch(mesg.getCode()){
    case SM.SM_popCMO:
      SM_popCMO(stream);
      break;

    case SM.SM_executeFunction:
      SM_executeFunction();
      break;

    case SM.SM_mathcap:
      SM_mathcap(stream);
      break;

    case SM.SM_setMathCap:
      SM_setMathCap(stream);
      break;

    default:
      System.out.println("received "+ mesg);
    }
  }

  private CMO CREATE(CMO[] argv){
    plotframe tmp = new plotframe(((CMO_INT32)argv[0]).intValue()
				  ,((CMO_INT32)argv[1]).intValue());

    for(int i=0;i<plotframe.size();i++){
      if(plotframe.elementAt(i) == null){
	plotframe.setElementAt(tmp,i);
	return new CMO_INT32(i);
      }
    }

    plotframe.addElement(tmp);
    plotframe.trimToSize();

    return new CMO_INT32(plotframe.size()-1);
  }

  private CMO PSET(CMO[] argv){
    plotframe tmp = (plotframe)plotframe.elementAt(((CMO_INT32)argv[0]).intValue());

    tmp.pset(((CMO_INT32)argv[1]).intValue(),
	     ((CMO_INT32)argv[2]).intValue(),
	     ((CMO_INT32)argv[3]).intValue());
    return new CMO_NULL();
  }

  private void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  private static String usage(){
    String ret = "";

    ret += "usage\t: java OMproxy [options]\n";
    ret += "options\t:\n";
    ret += "\t -h \t show this message\n";
    ret += "\t -host hostname \t (default localhost)\n";
    ret += "\t -data port \t (default 1300)\n";
    ret += "\t -control port \t (default 1200)\n";
    ret += "\t -insecure \t this version ignore this option\n";
    ret += "\t -debug \t display debug messages\n";

    return ret;
  }

  public static void main(String[] argv){
    String hostname = "localhost";
    int ControlPort = 1200, DataPort = 1300;
    OpenXMserver ox;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
        System.out.println("");
        System.exit(0);
      }else if(argv[i].equals("-host")){
        hostname = argv[++i];
      }else if(argv[i].equals("-data")){
        DataPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
        ControlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
        System.err.println("unknown option :"+ argv[i]);
        System.exit(1);
      }
    }

    //ox = new OpenXMserver(hostname,ControlPort,DataPort);
    ox = new OXplot(hostname,ControlPort,DataPort);
    /*
    try{
    }catch(java.net.UnknownHostException e){
      System.err.println("host unknown.");
      System.err.println(e.getMessage());
      return;
    }catch(IOException e){
      System.err.println("connection failed.");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }
    */

    ox.start();

    System.out.println("breaking...");
  }
}
