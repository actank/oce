#include "Weblog.h"

Weblog::Weblog()
{
	line = new char[10<<10];
}
Weblog::~Weblog()
{
	delete[] line;
}

// ProcNextLog {{{
// ret : 0 on success, -1 on failure 
int Weblog::ParseNextLog(FILE* fp)
{
	while (true)
	{
		fgets(line, 10<<10, fp);
		if (feof(fp))
			return -1;

		TrimLastEnter(line);
		//strtolower(line);
		//fprintf(stderr, "<%s>\n", line);

		if (line[0]==0)
			continue;

		int num = Parse(line);
		if (num<4)
			continue;

		date = atol(items[TIME])/1000;
		//weblog.Print();
		if (strcmp(items[num-2/*ACTION*/],"VISIT")!=0 && strcmp(items[num-2/*ACTION*/],"REPLY")!=0)
			continue;
		return 0;
	}
}// }}}

// Parse {{{
int Weblog::Parse(char* line)
{
	// initialize
	weight = 0;
	accepter1 = 0;
	accepter2 = 0;
	memset(items, 0, ITEM_SIZE*sizeof(char*));
	char* delem = " ";

	int num = split_c(line, items, ITEM_SIZE, delem);
	//if (num!=ITEM_MAX_NUM)
	//{
	//	fprintf(stderr, "num!=ITEM_MAX_NUM : ");
	//	for (int i=0; i<num; ++i)
	//		fprintf(stderr, "%s\t", items[i]);
	//	fprintf(stderr, "\n");
	//	return -1;
	//}
	if (num<4)
		return num;

	sender = atoi(items[UID]);
	weight = GetWeight(items[num-2/*ACTION*/], items[num-1/*OBJECT*/]);

	char* subitems[10];
	int subnum = split_c(items[num-1/*OBJECT*/], subitems, 10, "_");
	if (subnum>=2)
		accepter1 = atoi(subitems[subnum-1]);

	subnum = split_c(items[PATH], subitems, 10, "_");
	if (subnum>=2)
		accepter2 = atoi(subitems[1]);
	return num;
}// }}}

// GetWeight {{{
int Weblog::GetWeight(char* type, char* obj)
{
	if (strcmp(type,"REPLY")==0)
		return 3;
	else if (strncmp(obj, "PROFILE_", 8)==0)
		return 2;
	else
		return 1;
}// }}}

// Print {{{
int Weblog::Print() const
{
	for (int i=0; items[i]!=NULL; ++i)
		printf("<%s>", items[i]);
	printf("\n");
	return 0;
}// }}}

