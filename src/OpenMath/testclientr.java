/**
 * $OpenXM$
 *
 */

import java.io.*;
import ORG.openxm.tam.*;

final class testclientr{
  private OpenXM asir; // for debug

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

    try{ // connect to server
      asir = new OpenXM(host,CtrlPort,StreamPort,"ox_asir",8089,"");
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

    System.err.println("start");

    try{ // send data to server
      asir.send(new SM(SM.SM_mathcap));
      asir.send(new SM(SM.SM_popCMO));
      asir.send(new CMO_ZZ("12345678901234567890"));
      asir.send(new SM(SM.SM_popCMO));
    }catch(NumberFormatException e){
      System.err.println(e.getMessage());
    }catch(MathcapViolation e){
      System.err.println(e.getMessage());
    }

    try{ // receive a data which come from server
      while(true){
	OXmessage message = asir.receive();

	switch(message.getTag()){
	case OXmessage.OX_COMMAND:
          System.err.println("SM> "+ message.getBody());
          break;

	case OXmessage.OX_DATA:
	  System.err.println("CMO> "+ message.getBody());
          break;
	}
      }
    }catch(IOException e){
      e.printStackTrace();
    }finally{
      System.err.println("breaking...");
    }
  }
}
