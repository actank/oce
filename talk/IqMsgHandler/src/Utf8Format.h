
//#pragma pack(4) 

using namespace std;



/* Detection of invalid UTF-8 sequences is based on Table 3.1B
   of Unicode 3.2: http://www.unicode.org/unicode/reports/tr28/
   with the additional restriction of not allowing the Unicode
   code points 0xFFFF and 0xFFFE (sequences EF,BF,BF and EF,BF,BE).
   Implementation details:
     (A & 0x80) == 0     means A < 0x80
   and
     (A & 0xC0) == 0xC0  means A > 0xBF
*/

/*Invalidate code point [#x7F-#x84], [#x86-#x9F] */
#define UTF8_INVALID1(p) \
  ((*p) > 0x7F && ((p)[0] & 0x84 == 0)) || ((*p) > 0x86 && ((p)[0] & 0x9F == 0))
 
#define UTF8_INVALID2(p) \
  ((*p) < 0xC2 || ((p)[1] & 0x80) == 0 || ((p)[1] & 0xC0) == 0xC0)

#define UTF8_INVALID3(p) \
  (((p)[2] & 0x80) == 0 \
  || \
  ((*p) == 0xEF && (p)[1] == 0xBF \
  ? \
  (p)[2] > 0xBD \
  : \
  ((p)[2] & 0xC0) == 0xC0) \
  || \
  ((*p) == 0xE0 \
  ? \
  (p)[1] < 0xA0 || ((p)[1] & 0xC0) == 0xC0 \
  : \
  ((p)[1] & 0x80) == 0 \
  || \
  ((*p) == 0xED ? (p)[1] > 0x9F : ((p)[1] & 0xC0) == 0xC0)))

#define UTF8_INVALID4(p) \
  (((p)[3] & 0x80) == 0 || ((p)[3] & 0xC0) == 0xC0 \
  || \
  ((p)[2] & 0x80) == 0 || ((p)[2] & 0xC0) == 0xC0 \
  || \
  ((*p) == 0xF0 \
  ? \
  (p)[1] < 0x90 || ((p)[1] & 0xC0) == 0xC0 \
  : \
  ((p)[1] & 0x80) == 0 \
  || \
  ((*p) == 0xF4 ? (p)[1] > 0x8F : ((p)[1] & 0xC0) == 0xC0)))

/* 
 * reference from wiki
00000000-01111111 	00-7F 	0-127 		US-ASCII (single byte)
10000000-10111111 	80-BF 	128-191 	Second, third, or fourth byte of a multi-byte sequence
11000000-11000001 	C0-C1 	192-193 	Overlong encoding: start of a 2-byte sequence, but code point <= 127
11000010-11011111 	C2-DF 	194-223 	Start of 2-byte sequence
11100000-11101111 	E0-EF 	224-239 	Start of 3-byte sequence
11110000-11110100 	F0-F4 	240-244 	Start of 4-byte sequence
11110101-11110111 	F5-F7 	245-247 	Restricted by RFC 3629: start of 4-byte sequence for codepoint above 10FFFF
11111000-11111011 	F8-FB 	248-251 	Restricted by RFC 3629: start of 5-byte sequence
11111100-11111101 	FC-FD 	252-253 	Restricted by RFC 3629: start of 6-byte sequence
11111110-11111111 	FE-FF 	254-255 	Invalid: not defined by original UTF-8 specification
*/

#define UTF8_SINGLEBYTE(p) \
	(p)[0] >= 0x00 && (p)[0] <= 0x7F

#define UTF8_MULTIBYTES2(p) \
	(p)[1] >= 0x80 && (p)[1] <= 0xBF \
  	&& (p)[0] >= 0xC2 && (p)[0]<= 0xDF
 
#define UTF8_MULTIBYTES3(p) \
	(p)[0] >= 0xE0 && (p)[0]<= 0xEF	\
	&&(p)[1] >= 0x80 && (p)[1] <= 0xBF \
  	&&(p)[2] >= 0x80 && (p)[2] <= 0xBF


#define UTF8_MULTIBYTES4(p) \
	(p)[0] >= 0xF0 && (p)[0]<= 0xF4	\
	&&(p)[1] >= 0x80 && (p)[1] <= 0xBF \
  	&&(p)[2] >= 0x80 && (p)[2] <= 0xBF \
  	&&(p)[3] >= 0x80 && (p)[3] <= 0xBF 

inline static void Utf8Formatting(char* utf8str)
{
	int multiBytesNo = 0;
	char* pHead = (char*)utf8str;
	if(utf8str == NULL)
		return;
	int len = strlen(utf8str);
	if(len==0)
		return;

	while(pHead[0]!='\0')
	{
		len = strlen(utf8str);
		multiBytesNo = 0;
		if(UTF8_MULTIBYTES4((uint8_t*)pHead))
		{
			multiBytesNo = 4;
			if(UTF8_INVALID4((uint8_t*)pHead))
			{
				memmove(pHead, pHead+4, len+utf8str-pHead-4);
				memset((char*)(utf8str+len-4), 0, 4);	
			}
			else
				pHead+=4;
		}
		else if(UTF8_MULTIBYTES3((uint8_t*)pHead))
		{
			multiBytesNo = 3;
			if(UTF8_INVALID3((uint8_t*)pHead))
			{
				memmove(pHead, pHead+3, len+utf8str-pHead-3);
				memset((char*)(utf8str+len-3), 0, 3);	
			}
			else
				pHead+=3;
		}
		else if(UTF8_MULTIBYTES2((uint8_t*)pHead))
		{
			multiBytesNo = 2;
			if(UTF8_INVALID2((uint8_t*)pHead))
			{
				memmove(pHead, pHead+2, len+utf8str-pHead-2);
				memset((char*)(utf8str+len-2), 0, 2);	
			}
			else
			{
				pHead+=2;
			}
		}
		else if(UTF8_SINGLEBYTE((uint8_t*)pHead))
		{
			multiBytesNo = 1;
			pHead++;
		}

		if(multiBytesNo==0)
		{
			memmove(pHead, pHead+1, len+utf8str-pHead-1);
			memset((char*)(utf8str+len-1), 0, 1);
		}
	}
}

