/**
 * $OpenXM: OpenXM/src/OpenMath/oxclient.java,v 1.1 2000/11/15 05:51:57 tam Exp $
 *
 */

import ORG.openxm.tam.*;
import java.io.*;
import java.awt.event.*;

final class oxclient extends java.awt.Frame implements Runnable{
  private OpenXM server;
  private java.awt.TextArea con;
  private java.awt.TextField command;

  public oxclient(String host,int CtrlPort,int StreamPort){
    super("oxclient");
    add("North",con = new java.awt.TextArea(24,80));
    con.setEditable(false);
    {
      java.awt.Button button;
      add("West",button = new java.awt.Button("execute"));
      button.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e) {
	  try{
	    server.send(new CMO_STRING(command.getText()));
	    server.send(new SM(SM.SM_executeStringByLocalParser));
	    server.send(new SM(SM.SM_popString));
	  }catch(Exception exc){}
          command.setText("");
	}
      });
    }
    add("Center",command = new java.awt.TextField());
    pack();
    show();

    con.appendText("connecting...\n");
    try{ // connect to server
      server = new OpenXM(host,CtrlPort,StreamPort);
    }catch(java.net.UnknownHostException e){
      con.appendText("unknown host: "+ host +"\n");
      return;
    }catch(IOException e){
      con.appendText("connection failed.\n");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }
    con.appendText("connected\n");

    try{ // send data to server
      server.send(new SM(SM.SM_mathcap));
      server.send(new SM(SM.SM_popCMO));
      server.send(new CMO_ZZ("12345678901234567890"));
      server.send(new SM(SM.SM_popCMO));
    }catch(NumberFormatException e){
      System.err.println(e.getMessage());
    }catch(MathcapViolation e){
      System.err.println(e.getMessage());
    }catch(IOException e){
    }
    con.appendText("MATHCAP exchanged.\n");

    new Thread(this).start();
  }

  public void run(){
    try{ // receive a data which come from server
      while(true){
	OXmessage message = server.receive();

	con.appendText("> "+ message.getBody() +"\n");
      }
    }catch(IOException e){
      e.printStackTrace();
    }finally{
      System.err.println("breaking...");
    }
  }

  public static String usage(){
    return "\n";
  }

  public static void main(String[] argv) throws IOException{
    OpenXM asir;
    String host = "localhost";
    int CtrlPort = 1200,StreamPort = 1300;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
	System.err.print(usage());
	System.exit(0);
      }else if(argv[i].equals("-host")){
	host = argv[++i];
      }else if(argv[i].equals("-data")){
	StreamPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
	CtrlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
	System.err.println("unknown option :"+ argv[i]);
	System.exit(1);
      }
    }

    new oxclient(host,CtrlPort,StreamPort);
  }
}
