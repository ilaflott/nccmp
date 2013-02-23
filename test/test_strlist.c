#include <strlist.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define handle_error(status) {									\
		if (status != 0) {													\
			printf("Status fatal. Aborting.\n");			\
			exit(1);																	\
		}																						\
	}

int main(int argc, char* argv[])
{
	char test1[256];
	char test2[256];
	char test3[256];
	int len;
	int nlist1, nlist2, nlist3;
	char** list1 = NULL;
	char** list2 = NULL;
	char** list3 = NULL;
	char*p;
	
	strcpy(test1, "string1,string112,string3,string14,string5");
	strcpy(test2, "string3,string5");
	strcpy(test3, "string6,string7");

	printf("Testing newstringlist...\n");
	handle_error( newstringlist(&list1, &nlist1, 64) );
	handle_error( newstringlist(&list2, &nlist2, 64) );

	printf("Testing getstringlist...\n");
	getstringlist(test1, &list2, &nlist2); 

	printf("Testing instringlist 1...\n");
	handle_error(instringlist(list2, "string112", nlist2) != 1);

	printf("Testing copystrlist...\n");
	handle_error(copystrlist(list2, list1, nlist2, nlist1));

	printf("Testing addstringtolist...\n");	
	addstringtolist(list1, "string6added", nlist1);
	addstringtolist(list1, "string7added", nlist1);

	printf("Testing instringlist 2...\n");
	handle_error(instringlist(list1, "string6added", nlist1) != 1);

	handle_error( newstringlist(&list3, &nlist3, 64) );
	handle_error(copystrlist(list1, list3, nlist1, nlist3));	
	printf("Testing clearstringlist...\n");
	handle_error( clearstringlist(list1, nlist1) );
	printf("Testing freestringlist...\n");
	freestringlist(&list1, nlist1);
	
	printf("Testing printstrlist...\n");
	printf("Result:\n");
	printstrlist(list2, nlist2, stdout);
	printf("Expecting:\nstring1 string112 string3 string14 string5\n");
	printf("Testing printstrlist...\n");
	printf("Result:\n");
	printstrlist(list3, nlist3, stdout);
	printf("Expecting:\nstring1 string112 string3 string14 string5 string6added string7added (null) ...\n");

	printf("Testing strlistu...\n");
	handle_error( newstringlist(&list1, &nlist1, 64) );
	freestringlist(&list2, nlist2);
	handle_error( newstringlist(&list2, &nlist2, 64) );
	getstringlist(test2, &list2, &nlist2); 
	strlistu(list2, list3, list1, nlist2, nlist3, nlist1);
	printf("Result:\n");
	printstrlist(list1, nlist1, stdout);
	printf("Expecting:\nstring3 string5 string1 string112 string14 string6added string7added (null) ...\n");

	printf("Testing strlistsd...\n");
	handle_error( clearstringlist(list1, nlist1) );
	strlistsd(list2, list3, list1, nlist2, nlist3, nlist1);
	printf("Result:\n");
	printstrlist(list1, nlist1, stdout);
	printf("Expecting: (null) ...\n");
	
	printf("Testing appendstringtolist...\n");
	handle_error( appendstringtolist(&list1, "appended", &nlist1) );
	printf("Result:\n");
	printstrlist(list1, nlist1, stdout);
	printf("Expecting: (null) ... appended\n");	

	printf("Freeing memory...\n");
	freestringlist(&list1, nlist1);
	freestringlist(&list2, nlist2);
	freestringlist(&list3, nlist3);

	return 0;
}
