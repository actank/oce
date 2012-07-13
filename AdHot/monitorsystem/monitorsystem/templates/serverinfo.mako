<title>${c.value}</title>
<h1>${c.value}</h1>

<!--
<img src=${c.imgPath}>
-->

<%

for i in range(0,20):
  content = '<img src=/img/feednewstimeout_FeedNews'+ str(i)+".png"+">" 
  #context.write('<p>hello ${i}</p>')
  context.write(content);
  i = i+1
  
%>

