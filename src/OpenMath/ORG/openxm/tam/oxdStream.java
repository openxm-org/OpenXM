/* $OpenXM$ */
package ORG.openxm.tam;

import java.io.* ;
import java.net.*;
import java.util.*;
import java.lang.*;

public class oxdStream {
  private InputStream is = null;
  private OutputStream os = null;
  private Socket socket = null;
  private static long seed = 0;
  private static Random rrr = new Random(0);
 
  public oxdStream() {
	// dummy
  }
	
  public oxdStream(int port) throws IOException {
	  try {
		  socket = new Socket("localhost",port);
	  }catch(IOException e) {
		  System.err.println("The oxd server at localhost:"+port+" does not exist.\n");
	  }
	is = new BufferedInputStream(socket.getInputStream());
	os = socket.getOutputStream();
  }

  public oxdStream(String host,int port) throws IOException {
    socket = new Socket(host,port);
	is = new BufferedInputStream(socket.getInputStream());
	os = socket.getOutputStream();
  }

  public void write(String s) throws IOException {
	os.write(s.getBytes());
    os.flush();
  }

  public String getFileName(String s) {
	int i;
	int start = -1;
	int end;
	for (i=0; i<s.length(); i++) {
	  if (s.regionMatches(i,"file=\"",0,6)) {
		start = i+6;
		break;
	  }
	}
	if (start == -1) return null;
	end = s.length()-1;
	for (i=start; i<s.length(); i++) {
	  if (s.regionMatches(i,"\"",0,1)) {
		end = i;
	  }
	}
	return (new String(s.getBytes(),start,end-start) );
  }
  public String getTagName(String s) {
	int i;
	int start = -1;
	int end;
	for (i=0; i<s.length(); i++) {
	  if (s.regionMatches(i,"<",0,1)) {
		start = i+1;
		break;
	  }
	}
	if (start == -1) return null;
	end = s.length()-1;
	for (i=start; i<s.length(); i++) {
	  if (s.regionMatches(i,">",0,1)) {
		  end = i; break;
	  }
	  if (s.regionMatches(i,"/>",0,2)) {
		  end = i; break;
	  }
	  if (s.regionMatches(i," ",0,1)) {
		  end = i; break;
	  }
	}
	return (new String(s.getBytes(),start,end-start) );
  }

	public void touch(String fname) throws IOException {
		FileOutputStream fp = new FileOutputStream(fname);
		fp.write(0);
		fp.close();
	}
	
	public int getRandomPortNumber() {
		int fn = (int) Runtime.getRuntime().freeMemory(); // java.lang.Runtime.freeMemory()
		int p;
		if (seed == 0) {
			seed = (long) System.currentTimeMillis() // java.lang.System.Current...
				+ (long) fn;
			rrr = new Random(seed);
		}
		for (int i=0; i<fn % 1000; i++) rrr.nextInt();
		p = rrr.nextInt();
		if (p < 0) p = -p;
		return (2000 + (p % 6000));
	}
	
  public String readUntil(char u) throws IOException {
		String s = "";
		char c = 0;
		while (c != u) {
			c = (char) is.read();  // BUG
			s += c;
		}
		return s;
	}

	public int startPhase1() throws IOException {
		String s;
		String ans;
		String fname;
		int port;
		
		readUntil('?');
		s = "<login method=\"file\">\n";
		System.err.println(s);
		write(s);

		ans = readUntil('?');
		System.err.println(ans);
		if (!getTagName(ans).equals("challenge")) {
			; // not yet written
		}
		port = getRandomPortNumber();
		fname = getFileName(ans);
		touch(fname);

		s = "<done/>\n";
		System.err.println(s);
		write(s);
		ans = readUntil('?');
		return port;
	}
    // Open the port and port2 for listenning.
	public void startPhase2(String oxname,int port) throws IOException {
		String s;
		String ans;
		String fname;
		int port2;
		port2 = port+1;
		
        s="<launch> ox -ox "+oxname+" -reverse -control "+port+" -data "+port2 
		  +	" </launch> \n";
		System.err.println(s);
		write(s);
	}

  public static void main0(String[] argv) throws IOException {
 	 oxdStream oxd = new oxdStream();
     String s = "<challenge file=\"/home/nobuki/.oxd123\"/>";
     System.err.println( oxd.getFileName(s) );
     System.err.println( oxd.getTagName(s) );
     System.err.println( oxd.getRandomPortNumber() );
     System.err.println( oxd.getRandomPortNumber() );
     System.err.println( oxd.getRandomPortNumber() );
  }
  public static void main(String[] argv) throws IOException {
	  int port;
 	 oxdStream oxd = new oxdStream(8089);
	 port = oxd.startPhase1();
     // start for listenning.
	 oxd.startPhase2("ox_asir",port);
  }
}

