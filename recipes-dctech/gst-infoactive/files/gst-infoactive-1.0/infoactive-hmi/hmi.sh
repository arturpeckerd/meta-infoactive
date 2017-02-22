#!/bin/sh
### BEGIN INIT INFO
# Provides:             HMI
# Required-Start:       
# Required-Stop:        
# Default-Start:        2 3 4 5
# Default-Stop:         0 1 6
# Short-Description:    Graphical user interface
### END INIT INFO

case "$1" in
"start")
        hmi &
	;;
"stop")
        killall -SIGINT sbengine
        killall -9 hmi-sbio
        ;;
*)
        echo "usage: $0 [start | stop]"
        ;;
esac
