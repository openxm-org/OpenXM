/**
 * PolyCalc.java $OpenXM$
 */

import JP.ac.kobe_u.math.tam.OpenXM.*;
import java.applet.*;
import java.awt.*;

class PolyCalc extends Applet{
  private String host;
  int ControlPort,DataPort;
  private OpenXM oxm;
  private Button random1_button,random2_button;
  private Button mul_button,remainder_button,swap_button,set_button;
  private TextField poly1,poly2;
  private TextArea textarea;
  //private Text

  PolyCalc(String host,int ControlPort,int DataPort){
    this.host = host;
    this.ControlPort = ControlPort;
    this.DataPort = DataPort;
  }

  public void init(){
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();

    //setFont();
    setLayout(gridbag);

    c.fill = GridBagConstraints.BOTH;

    textarea = new TextArea();
    textarea.setEditable(false);
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(textarea,c);
    c.gridwidth = 1;
    add(textarea);

    {
      Label label = new Label("poly 1:");
      gridbag.setConstraints(label,c);
      add(label);
    }

    random1_button = new Button("generate random polynomial");
    gridbag.setConstraints(random1_button,c);
    add(random1_button);

    mul_button = new Button("poly1 * poly2"); 
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(mul_button,c);
    c.gridwidth = 1;
    add(mul_button);

    poly1 = new TextField(20);
    c.gridwidth = 2;
    gridbag.setConstraints(poly1,c);
    c.gridwidth = 1;
    add(poly1);

    remainder_button = new Button("poly1 % poly2");
    //c.gridx = 2;
    //c.weightx = 0.0;
    gridbag.setConstraints(remainder_button,c);
    add(remainder_button);

    swap_button = new Button("swap poly1 & poly2");
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(swap_button,c);
    c.gridwidth = 1;
    add(swap_button);

    {
      Label label = new Label("poly 2:");
      gridbag.setConstraints(label,c);
      add(label);
    }

    random2_button = new Button("generate random polynomial");
    gridbag.setConstraints(random2_button,c);
    add(random2_button);

    set_button = new Button("poly1 <= poly2");
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints(set_button,c);
    c.gridwidth = 1;
    add(set_button);

    poly2 = new TextField();
    c.gridwidth = 2;
    gridbag.setConstraints(poly2,c);
    c.gridwidth = 1;
    add(poly2);
  }

  public void start(){
    textarea.append("Connecting to "+ host
		 +"("+ ControlPort +","+ DataPort +")\n");

    try{
      oxm = new OpenXM(host,ControlPort,DataPort);
    }catch(java.io.IOException e){
      textarea.append("failed.\n");
      stop();
      return;
    }
    textarea.append("Connected.\n");
  }

  public static void main(String argv[]){
    Frame frame = new Frame("Polynomial Calculator");
    Applet applet;
    String host = "localhost";
    int DataPort = 1300, ControlPort = 1200;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
        System.out.println("");
        System.exit(0);
      }else if(argv[i].equals("-host")){
        host = argv[++i];
      }else if(argv[i].equals("-data")){
        DataPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
        ControlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
        System.err.println("unknown option :"+ argv[i]);
        System.exit(1);
      }
    }
    applet = new PolyCalc(host,ControlPort,DataPort);

    applet.init();
    frame.add("Center",applet);
    frame.pack();
    frame.setSize(frame.getPreferredSize());
    frame.show();
    applet.start();
  }
}
