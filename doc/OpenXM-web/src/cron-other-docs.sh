#!/bin/sh
OpenXM_web_doc=/home/web/OpenXM/Current/doc/other-docs
(cd ${OpenXM_web_doc} ; ./os_muldif-get.sh)
(cd ${OpenXM_web_doc} ; ./oxgen-index >index.html)
# execute this as a daily cron in the user level.
# crontab -e and add
# 30 10 * * * /home/web/OpenXM/Current/doc/other-docs/cron-other-docs.sh
#

