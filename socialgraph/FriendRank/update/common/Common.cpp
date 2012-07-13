#include "Common.h"

time_t GetYestodayZero(time_t now)
{
	now -= 24*3600;  // 得到昨天此时的秒数
	struct tm *ltime = localtime(&now);
	ltime->tm_hour = 0;
	ltime->tm_min = 0;
	ltime->tm_sec = 0;
	return mktime(ltime);
}

// split_c(char* str, char** items, int item_size, const char* delim) {{{
int split_c(char* str, char** items, int item_size, const char* delim)
{
	assert(str && items);

	char* begin = str;
	char* end   = NULL;
	int   cnt   = 0;

	while (*begin != '\0')
	{
		// find the word end.
		end = begin;
		while (*end!='\0' && strchr(delim, *end)==NULL)
			++end;

		if(cnt+1 == item_size)
			return -1;
		items[cnt++] = begin;

		if(*end=='\0')
			break;

		*end = 0;
		begin = end+1;
	}
	return cnt;
}// }}}

// TrimLastEnter {{{
char* TrimLastEnter(char* str)
{
	char* p = str;
	while (*p!=0)
		++p;
	--p;

	while (p>=str)
	{
		if (*p=='\r' || *p=='\n')
			*p = 0;
		else
			break;
		--p;
	}
	return str;
}// }}}

// TravesalDir {{{
int TravesalDir(const char* dir, set<string>& fnames)
{
	DIR * pDir;
	struct dirent * pEntry;
	struct stat statbuf;
	if ((pDir=opendir(dir)) == NULL)
	{
		fprintf(stderr, "can't open directory:%s", dir);
		return -1;
	}

	char oridir[512];
	getcwd(oridir,510);
	chdir(dir);    // 有副作用，需要还原当前工作目录
	while ((pEntry=readdir(pDir)) != NULL)
	{
		stat(pEntry->d_name, &statbuf);
		if (S_ISREG(statbuf.st_mode))
		{
			//printf("%s\n", pEntry->d_name);
			fnames.insert(pEntry->d_name);
		}
	}
	chdir(oridir);  // 还原当前工作目录
	closedir(pDir);
	return 0;
}// }}}

// PrintTime {{{
void PrintTime()
{
	time_t now = time(NULL);
	char date_str[20];
	time2str(now, date_str);
	fprintf(stderr, "+------------------------------+\n");
	fprintf(stderr, "| %s          |\n", date_str);
	fprintf(stderr, "+------------------------------+\n");
}// }}}

// get_filesize {{{
long get_filesize(char *filename)
{
	struct stat f_stat;
	if (stat(filename, &f_stat) == -1)
	{
		return -1;
	}
	return (long)f_stat.st_size;
}// }}}
// time2str {{{
char* time2str(const time_t ltime, char* str)
{
	assert(str!=NULL);

	struct tm *Tm = localtime(&ltime);
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d",
			Tm->tm_year+1900,
			Tm->tm_mon+1,
			Tm->tm_mday,
			Tm->tm_hour,
			Tm->tm_min,
			Tm->tm_sec);
	return str;
}// }}}
