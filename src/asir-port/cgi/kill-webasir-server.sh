#!/bin/sh
echo "This script is for restarting the server for the webasir."
echo "When you rewrite programs in asir, you need to restart the servers."
echo "Side effect: this kills all sm1 process, too!!"
${OpenXM_HOME}/src/kan96xx/Doc/httpd-asir2-kill.sh
/usr/bin/killall sm1
echo "It will be a good idea to execute these commands in the /etc/crontab."
