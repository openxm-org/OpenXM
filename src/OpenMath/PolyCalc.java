/**
 * $OpenXM: OpenXM/src/OpenMath/PolyCalc.java,v 1.13 2005/07/17 02:48:21 takayama Exp $
 */

/*
   How to try.
    ox -ox ox_sm1
    java PolyCalc
    Input two polynomials in x and y  and Press the grobner base button.
    Actions for other buttons have not yet been implemented. 
 */

import ORG.openxm.tam.*;
import java.applet.*;
import java.awt.event.*;
import java.awt.*;

class PolyCalc extends Applet implements ActionListener,Runnable{
    private String host = "localhost";
    private int ControlPort = 1200,DataPort = 1300;
    private OpenXM oxm;
    //private Button random1_button,random2_button;
    //private Button mul_button,remainder_button,swap_button,set_button;
    private TextField poly1,poly2;
    private TextArea textarea;
    private Thread thread = null;
    private boolean debug = false;
    //private Text

    PolyCalc(String host,int ControlPort,int DataPort){
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

        textarea = new TextArea();
        textarea.setEditable(false);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(textarea,c);
        c.gridwidth = 1;
        add(textarea);

        {
            Label label = new Label("poly A:");
            gridbag.setConstraints(label,c);
            add(label);
        }

        button = new Button("generate random polynomial A");
        button.addActionListener(this);
        gridbag.setConstraints(button,c);
        add(button);

        button = new Button("A * B"); 
        button.addActionListener(this);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(button,c);
        c.gridwidth = 1;
        add(button);

        poly1 = new TextField(20);
        poly1.addActionListener(this);
        c.gridwidth = 2;
        gridbag.setConstraints(poly1,c);
        c.gridwidth = 1;
        add(poly1);

        button = new Button("A % B");
        button.addActionListener(this);
        //c.gridx = 2;
        //c.weightx = 0.0;
        gridbag.setConstraints(button,c);
        add(button);

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

        button = new Button("generate random polynomial B");
        button.addActionListener(this);
        gridbag.setConstraints(button,c);
        add(button);

        button = new Button("poly1 <= poly2");
        button.addActionListener(this);
        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(button,c);
        c.gridwidth = 1;
        add(button);

        poly2 = new TextField();
        c.gridwidth = 2;
        gridbag.setConstraints(poly2,c);
        c.gridwidth = 1;
        add(poly2);

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
                OXmessage tmp = oxm.receive();

                textarea.append("=> "+ tmp.getBody() +"\n");
                Thread.yield();
            }
        }catch(java.io.IOException e){}
    }

    public void actionPerformed(ActionEvent evt) {
        String arg = evt.getActionCommand();

        debug("press \""+ arg +"\" button.");

        if(arg.equals("quit")){
        }else if(arg.equals("grobner base")){
            try{
                debug("poly A: "+ poly1.getText());
                oxm.send(new CMO_STRING("[[("+ poly1.getText() +") ("+ poly2.getText() +")] (x,y)] gb"));
                oxm.send(new SM(SM.SM_executeStringByLocalParser));
                oxm.send(new SM(SM.SM_popString));
            }catch(java.io.IOException e){
            }catch(MathcapViolation e){
            }
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
        textarea.append("Connecting to "+ host
                        +"("+ ControlPort +","+ DataPort +")\n");

        try{
            /*
              Runtime r = Runtime.getRuntime();

              r.exec("ox -ox ox_sm1");
            */
            oxm = new OpenXM(host,ControlPort,DataPort);
            textarea.append("Connected.\n");
            oxm.send(new CMO_STRING("[(parse) (cohom.sm1) pushfile] extension ;\n"));
            oxm.send(new SM(SM.SM_executeStringByLocalParser));

            thread = new Thread(this);
            thread.start();
        }catch(java.io.IOException e){
            textarea.append("failed.\n");
            stop();
        }catch(MathcapViolation e){
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
        PolyCalc applet;
        String host = "localhost";
        int DataPort = 1300, ControlPort = 1200;

        for(int i=0;i<argv.length;i++){
            if(argv[i].equals("-h")){
                System.err.print(usage());
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
        applet = new PolyCalc(host,ControlPort,DataPort);
        applet.debug = true;

        applet.init();
        frame.add("Center",applet);
        frame.pack();
        frame.setSize(frame.getPreferredSize());
        frame.show();
        applet.start();
    }
}
