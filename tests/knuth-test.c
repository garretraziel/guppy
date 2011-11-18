#include <stdio.h>
#include "ial.h"
#include "string.h"

int main(void)
{
  int c;
  string str;
  string str2;
  string *nacitani = &str;
  int nacteno = 0;
  if(str_new(&str, 255) == 0)
    return 1;
  if(str_new(&str2, 255) == 0) {
    str_clean(&str);
    return 1;
  }
  while((c = getchar()) != EOF) {
    if(c == '\n'){
      if (nacteno == 1) {
	printf("%d-----\n",find2(&str,&str2));
	printf("A co nam rekne pan JMH... %d-----\n",find(&str,&str2));
	str_clean(&str);
	str_clean(&str2);
	nacitani = &str;
	nacteno = 0;
      } else {
	nacteno = 1;
	nacitani = &str2;
      }
    } else {
      str_push(nacitani, c);
    }
  }
  return 0;
}
