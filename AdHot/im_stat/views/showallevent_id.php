<!DOCTYPE html>
<html lang="en">
<head>
</head>
<body>
<table border="1">
<?php 
$this->load->library('table');

foreach ($data as $key => $value) {

 echo '<tr>';
 echo '<td>'.$value['event_id'].'</td>';
 echo '<td style="width:350px">'.$value['describe'].'</td>';
 echo '<td style="width:100px">'.$value['aggregate_type'].'</td>';
 
 echo '<td style="width:100px">'.$value['value_type'].'</td>';
 
 if($value['value_type'] == '0')
 {
   echo '<td style="width:80px">';
   $url = sprintf("<a href='/event/eventid_update/%s/%s'>start</a>",$value['event_id'],$value['value_type']);
   echo $url;
   echo '</td>';
 }
 else
 {
   echo '<td style="width:80px">';
   $url = sprintf("<a href='/event/eventid_update/%s/%s'>end</a>",$value['event_id'],$value['value_type']);
   echo $url;
   echo '</td>';
 }
 
 echo '</tr>';
}
?>
</table>

</body>
</html>
