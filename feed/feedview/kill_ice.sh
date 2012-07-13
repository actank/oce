for i in `cat h.so`; do echo $i; ssh $i "pkill -f FeedView"; done
