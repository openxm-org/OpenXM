/**
 * $OpenXM: OpenXM/src/OpenMath/elimi.java,v 1.3 1999/12/09 00:32:36 tam Exp $
 */

import JP.ac.kobe_u.math.tam.OpenXM.*;
import java.applet.*;
import java.awt.event.*;
import java.awt.*;
import java.util.Vector;

class elimi extends Applet implements ActionListener,Runnable{
  private String host = "localhost";
  private int ControlPort = 1200,DataPort = 1300;
  private OpenXM oxm;
  private TextField poly1,poly2;
  private TextArea input,output;
  private Thread thread = null;
  private boolean debug = false;

  elimi(String host,int ControlPort,int DataPort){
    this.host = host;
    this.ControlPort = ControlPort;
    this.DataPort = DataPort;
  }

  public void init(){
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    Button button;

    //setFont();
    setLayout(gridbag);

    c.fill = GridBagConstraints.BOTH;

    {
      Label label = new Label("input polynomials");
      c.gridwidth = 2;
      gridbag.setConstraints(label,c);
      c.gridwidth = 1;
      add(label);

      label = new Label("outputs");
      c.gridwidth = GridBagConstraints.REMAINDER;
      gridbag.setConstraints(label,c);
      c.gridwidth = 1;
      add(label);
    }

    input = new TextArea(10,40);
    input.setEditable(true);
    c.gridwidth = 2;
    gridbag.setConstraints(input,c);
    c.gridwidth = 1;
    add(input);

    output = new TextArea(10,40);
    output.setEditable(false);
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(output,c);
    c.gridwidth = 1;
    add(output);

    poly1 = new TextField(20);
    poly1.addActionListener(this);
    c.gridwidth = 2;
    gridbag.setConstraints(poly1,c);
    c.gridwidth = 1;
    add(poly1);

    button = new Button("swap A & B");
    button.addActionListener(this);
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(button,c);
    c.gridwidth = 1;
    add(button);

    {
      Label label = new Label("poly B:");
      gridbag.setConstraints(label,c);
      add(label);
    }

    button = new Button("poly1 <= poly2");
    button.addActionListener(this);
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(button,c);
    c.gridwidth = 1;
    add(button);

    button = new Button("grobner base");
    button.addActionListener(this);
    gridbag.setConstraints(button,c);
    add(button);

    button = new Button("quit");
    button.addActionListener(this);
    gridbag.setConstraints(button,c);
    add(button);
  }

  public void run(){ // for debug
    try{
      while(true){
        CMO tmp;

        Thread.yield();

        switch(oxm.receiveOXtag()){
        case OpenXM.OX_COMMAND:
          oxm.receiveSM();
          break;

        case OpenXM.OX_DATA:
          tmp = oxm.receiveCMO();
          System.out.println("=> "+ tmp);
          break;
        }
      }
    }catch(java.io.IOException e){}
  }

  public void actionPerformed(ActionEvent evt) {
    String arg = evt.getActionCommand();

    debug("press \""+ arg +"\" button.");

    if(arg.equals("quit")){
    }else if(arg.equals("grobner base")){
      Vector polys = new Vector(),variables = new Vector();

      try{
	java.io.StringReader in = new java.io.StringReader(input.getText());
	String poly,variable,com;
	char a = 0;

	while(a != (char)-1){
	  poly = "";
	  variable = "";
	  while((a=(char)in.read()) != (char)-1 && a != '\n'){
	    //debug("read :"+(int)a);
	    if(Character.isLetter(a) ||
	       (Character.isDigit(a) && !variable.equals(""))){
	      variable += a;
	    }else if(!variable.equals("")){
	      debug("add variable:" + variable);
	      variables.addElement(variable);
	      variable = "";
	    }
	    if(!Character.isWhitespace(a)){
	      poly += a;
	    }
	  }
	  if(!variable.equals("")){
	    debug("add variable:" + variable);
	    variables.addElement(variable);
	  }
	  if(!poly.equals("")){
	    debug("read poly:"+ poly);
	    polys.addElement(poly);
	  }
	}

	debug("poly A: "+ poly1.getText());
	com = "[[";
	while(!polys.isEmpty()){
	  com += "("+ polys.elementAt(0) +")";
	  polys.removeElementAt(0);
	  if(!polys.isEmpty()){
	    com += " ";
	  }
	}
	com += "] (";
	while(!variables.isEmpty()){
	  Object tmp = variables.elementAt(0);

	  com += tmp;
	  while(variables.removeElement(tmp)){};
	  if(!variables.isEmpty()){
	    com += ",";
	  }
	}
	com += ")] gb";

	debug("command: "+ com);
	oxm.send(new CMO_STRING(com));
	oxm.send(new SM(SM.SM_executeStringByLocalParser));
	oxm.send(new SM(SM.SM_popString));
      }catch(java.io.IOException e){}
    }
    /*
      if ("first".equals(arg)) {
      ((CardLayout)cards.getLayout()).first(cards);
      } else if ("next".equals(arg)) {
      ((CardLayout)cards.getLayout()).next(cards);
      } else if ("previous".equals(arg)) {
      ((CardLayout)cards.getLayout()).previous(cards);
      } else if ("last".equals(arg)) {
      ((CardLayout)cards.getLayout()).last(cards);
      } else {
      ((CardLayout)cards.getLayout()).show(cards,(String)arg);
      }
      */
  }

  public void start(){
    System.out.println("Connecting to "+ host
		       +"("+ ControlPort +","+ DataPort +")");

    try{
      Runtime runtime = Runtime.getRuntime();

      runtime.exec("sh -c \"xterm -name $OpenXM_HOME\"");
      //runtime.getInputstream();
      //runtime.getOutput();
      //runtime.exec("sh -c \"/home/tam/OpenXM/lib/sm1/bin/oxlog /usr/X11R6/bin/xterm -name echo ${OpenXM_HOME} /home/tam/OpenXM/lib/sm1/bin/ox -ox /home/tam/OpenXM/lib/sm1/bin/ox_sm1_forAsir -data "+ DataPort +" -control "+ ControlPort +"\"");

      Thread.sleep(3000);

      oxm = new OpenXM(host,ControlPort,DataPort);
      System.out.println("Connected.");
      oxm.send(new CMO_STRING("(cohom.sm1) run ;\n"));
      oxm.send(new SM(SM.SM_executeStringByLocalParser));

      thread = new Thread(this);
      thread.start();
    }catch(Exception e){
      System.out.println("failed.");
      stop();
    }
  }

  private void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  private static String usage(){
    String ret = "";

    ret += "usage\t: java PolyCalc [options]\n";
    ret += "options\t:\n";
    ret += "\t -h \t show this message\n";
    ret += "\t -host hostname \t (default localhost)\n";
    ret += "\t -data port \t (default 1300)\n";
    ret += "\t -control port \t (default 1200)\n";
    ret += "\t -debug \t display debug message\n";

    return ret;
  }

  public static void main(String argv[]){
    Frame frame = new Frame("Polynomial Calculator");
    //Applet applet;
    elimi applet;
    String host = "localhost";
    int DataPort = 1300, ControlPort = 1200;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
        System.out.print(usage());
        System.exit(0);
      }else if(argv[i].equals("-debug")){
	//debug = true;
      }else if(argv[i].equals("-host")){
        host = argv[++i];
      }else if(argv[i].equals("-data")){
        DataPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
        ControlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
        System.err.println("unknown option : "+ argv[i]);
        System.err.println("");
        System.err.print(usage());
        System.exit(1);
      }
    }
    applet = new elimi(host,ControlPort,DataPort);
    applet.debug = true;

    applet.init();
    frame.add("Center",applet);
    frame.pack();
    frame.setSize(frame.getPreferredSize());
    frame.show();
    applet.start();
  }
}
