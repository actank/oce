

for i in `cat h.so`;
do
  echo $i;
  scp $1 $i:~/XFeed/etc/feed_view_tpl/;
  ssh $i "cd ~/XFeed/etc/feed_view_tpl/; tar zxf $1; rm -f $1; pkill -60 -f FeedView"
done
for i in `cat h.cwfd`;
do 
  echo $i;
  scp $1 $i:/data/cwf/tpl;
  ssh $i "cd /data/cwf/tpl/; tar zxf $1; rm -f $1; pkill -60 cwfd"
done
