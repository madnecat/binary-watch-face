#ifndef DATES_H
#define DATES_H

#include <string.h>
#include <stdlib.h>

//table contenant les suffixes des dates
char *date_suffixe[31];

void init_suffixes(){
	for(int i = 0; i<31; i++) {
		date_suffixe[i] = "th";	
	}
	date_suffixe[0] = "st";
	date_suffixe[1] = "nd";
	date_suffixe[2] = "rd";
	date_suffixe[20] = "st";
	date_suffixe[21] = "nd";
	date_suffixe[22] = "rd";
	date_suffixe[30] = "st";
}

char* concat(char *s1, char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

#endif