/* $OpenXM$ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set OpenXM_HOME for CD */
main() {
	char s[1024];
	char drive = 'c';
	char *systemdrive;
	getcwd(s,1023);
	if (strstr(s,"/cygdrive/")==s) {
		drive = s[10];  /* drive letter */
	}else{
		systemdrive = getenv("SystemDrive");
		if (systemdrive == NULL) {
			systemdrive = getenv("SYSTEMDRIVE");
		}
		if (systemdrive != NULL) {
			drive = systemdrive[0];
		}
	}
	printf("set OpenXM_HOME=/cygdrive/%c/OpenXM-win\n",drive);
	printf("set OpenXM_HOME_WIN=%c:\\OpenXM-win\n",drive);
}

/* Generate batch file a.bat and  "call a " */


