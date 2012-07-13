#!/bin/bash
. /etc/profile
/usr/local/bin/python monitorAllService.py
/usr/local/bin/python adapter.py
/usr/local/bin/python feed.py
/usr/local/bin/python antispam.py
