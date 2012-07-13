class ImageInfo:
  def __init__(self):
    self.img_path = ''
    self.chart_name = ''
    self.big_photo_url = ''

class Util:
  def getChartPrefix(self, chartname):
    i = len(chartname)
    num = 0;
    while(i >= 0):
      pos = i - 1;
      if(chartname[pos].isdigit()):
        num -= 1;
        i -= 1;
      else:
        break;
          
     
    prefix_name = chartname[0:num]
    return prefix_name;  
      
