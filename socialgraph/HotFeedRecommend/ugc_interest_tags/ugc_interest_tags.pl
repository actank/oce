#!/usr/bin/perl

%owner_ugc_info;
%ugc_tags_map;

#open owner_ugc_list, "/data/xce/Project/hotfeedrecommend/data/oo";
open owner_ugc_list, "/data/xce/Project/hotfeedrecommend/data/owner_ugc_list";
open ugc_tags, "/data/xce/Project/hotfeedrecommend/data/ugc_tag";
open output_file, "> /data/xce/Project/hotfeedrecommend/ugc_interest_tags/ugc_list_interest";

print "/data/xce/Project/hotfeedrecommend/data/owner_ugc_list\n";
print "/data/xce/Project/hotfeedrecommend/data/ugc_tag\n";
print "> /data/xce/Project/hotfeedrecommend/ugc_interest_tags/ugc_list_interest\n";

while ($line=<owner_ugc_list>) {
	chomp($line);
	if (($ugc_id) = $line =~ m/[0-9]+	([0-9]+)	[0-9]+	[0-9]+	[0-9]+	[0-9]+/g) {
#		print $ugc_id." : ".$line."\n";
		$owner_ugc_info{$ugc_id} = $line;
	}
}

#while (($key, $value) = each %owner_ugc_info) {
#	print $key." : ".$value."\n";
#}

while ($line=<ugc_tags>) {
	chomp($line);
	if (($ugc_id, $type) = $line =~ m/([0-9]+)	\S+	(\S+)/g) {
#		print $ugc_id."\t".$type."\n";	
		$ugc_tags_map{$ugc_id} = $type;
	}
}

#while (($key, $value) = each %ugc_tags_map) {
#	print $key." : ".$value."\n";
#}

while (($key, $value) = each %owner_ugc_info) {
	if (exists $ugc_tags_map{$key}) {
		syswrite(output_file, $value."\t".$ugc_tags_map{$key}."\n");
	} else {
		syswrite(output_file, $value."\t"."NULL"."\n");	
	}
}
