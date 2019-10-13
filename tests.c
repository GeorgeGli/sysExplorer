
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "strutils.h"
#include "hashtable.h"
#include "sysexplorer.h"
#include "urldecode.h"

#undef NDEBUG
#include <assert.h>


#define RUN(func) { printf("%s\n", #func ); func(); }


/*
  This suite contains tests for linkedlist and strutils.
 */


void test_trim_case(char* (*func)(const char*), const char* s, const char* es)
{
	char* ts = func(s);
	assert(strcmp(ts, es)==0);
	free(ts);
}


void test_trim()
{
	
	test_trim_case(strtrim, " \n\t  \f  ", "");
	test_trim_case(strtrim, " adasd asdas \n", "adasd asdas");
	test_trim_case(strtrim, "", "");
	test_trim_case(strltrim, "asd   ", "asd   ");
	test_trim_case(strltrim, "  asd   ", "asd   ");
	test_trim_case(strrtrim, "asd   ", "asd");
	test_trim_case(strrtrim, "  asd   ", "  asd");
}


void test_split_join_case(const char* input, const char* sep, int efrags, ...)
{
	List mylist;
	int frags;
	va_list ap;
	ListNode p;
	char* arg;

	List_init(&mylist);
	frags = strsplit(input, sep, &mylist);
	
	assert(efrags==frags);

	va_start(ap, efrags);

	p = List_begin(&mylist);
	while(1) 
	{
		arg = va_arg(ap, char*);
		if(arg==NULL) break;
		assert(p != List_end(&mylist));
		assert(strcmp(arg, (const char*) p->data)==0);
		p = p->next;
	}
	va_end(ap);

	assert(p==List_end(&mylist));
	if(efrags) {
		arg = strjoin(sep, List_begin(&mylist), List_end(&mylist));
		assert(strcmp(input, arg)==0);
		free(arg);
	}

	for(p=List_begin(&mylist); p!=List_end(&mylist); p=p->next)
		free(p->data);
	List_clear(&mylist);
}


void test_split_join()
{
	test_split_join_case("abccbded","b",3,"a","cc","ded",NULL);
	test_split_join_case("abaaAAB","AAB",2,"abaa","",NULL);
	test_split_join_case("AABaabAAB","AAB",3,"","aab","",NULL);
	test_split_join_case("AABaabAAB","AAb",1,"AABaabAAB",NULL);
	test_split_join_case("asdasd","",0,NULL);
}


void test_strconcat_case(const char* s1, char* s2)
{
	assert(strcmp(s1,s2)==0);
	free(s2);
}


void test_strconcat()
{
	test_strconcat_case("abc", strconcat("a","bc",NULL));
	test_strconcat_case("abc", strconcat("ab","c",NULL));
	test_strconcat_case("abc", strconcat("abc",NULL));
	test_strconcat_case("abc", strconcat("a","b","c",NULL));
	test_strconcat_case("abc", strconcat("","a","b","","c",NULL));	
	test_strconcat_case("", strconcat("","",NULL));
	test_strconcat_case("", strconcat("",NULL));
}

int strlisteq(List* l, ...)
{
	va_list ap;
	ListNode p;
	int ret = 1;

	const char *sa;

	p=List_begin(l);
	va_start(ap, l);
	
	while(1) {
		sa = va_arg(ap, const char*);
		if(sa == NULL) {
			if(p != List_end(l)) ret=0;
			break;
		}
		if(p == List_end(l) || strcmp((char*) p->data, sa)!=0) { 
			ret=0; break; 
		}
		p = p->next;
	}

	va_end(ap);
	return ret;
}

void test_sort()
{
	static const char* strs[5] = { "a","b", "c", "d","e" };
	List L1, L2;
	int i,j;

	for(i=0;i<5;i++)
		for(j=0;j<5;j++) 
		{
			int p,q;
			p = (i<j) ? 1: 0;
			q = strless(strs[i],strs[j]) ? 1 :0;
			assert(p==q);
	        }

	List_init(&L1); 
	List_init(&L2);

	/* Add to L1, L2 */
	for(i=0;i<5;i++) {
		List_push_back(&L1,(void*) strs[i]);
		List_push_back(&L2,(void*) strs[3*(i+3) % 5]);
	}

	List_sort(&L1, (int (*)(void*,void*)) strless);
	List_sort(&L2, (int (*)(void*,void*)) strless);

	assert( strlisteq(&L1, "a", "b", "c" ,"d", "e", NULL));
	assert( strlisteq(&L2, "a", "b", "c" ,"d", "e", NULL));

	List_clear(&L1);
	List_clear(&L2);
}


void testsuite1()
{
	RUN(test_trim);
	RUN(test_split_join);
	RUN(test_strconcat);
	RUN(test_sort);
}

/*
 * Suite for testing hashtable
 */

#define HDATA 25
const char* hdata[HDATA] = {
		"a1", "b1", "c1", "d1", "e1",
		"a2", "b2", "c2", "d2", "e2",
		"a3", "b3", "c3", "d3", "e3",
		"a4", "b4", "c4", "d4", "e4",
		"a5", "b5", "c5", "d5", "e5"
};

size_t str2hash(const char* s)
{
	return 131*s[0]+171*s[1];
}

void load_ht(HashTable *ht, size_t n) {
	size_t i;
	HashTable_init(ht, n, (HashFunc)str2hash, (EqPred)streq);
	for(i=0; i<HDATA; i++)
		HashTable_insert(ht, hdata[i], hdata[i]);
}

void test_build()
{
	HashTable ht;
	load_ht(&ht, 20);

	assert(HashTable_size(&ht)==25);

	HashTable_clear(&ht);
}

void test_find()
{
	HashTable ht;
	load_ht(&ht, 20);

	assert(HashTable_find(&ht,"a3")!=HashTable_end(&ht));
	assert(streq(HashTable_find(&ht,"a3")->key, "a3" ));
	assert(streq(HashTable_find(&ht,"a3")->value, "a3" ));

	HashTable_clear(&ht);
}

void test_remove()
{
	HashTable ht;
	load_ht(&ht, 20);

	assert(HashTable_remove(&ht,"a3"));
	assert(! HashTable_remove(&ht,"a3"));
	assert(! HashTable_remove(&ht,"z3"));
	assert( HashTable_size(&ht)==24 );

	HashTable_clear(&ht);
	assert(HashTable_size(&ht)==0);
}

void testsuite2()
{
	RUN(test_build);
	RUN(test_find);
	RUN(test_remove);
}

void test_urldecode()
{
	assert(streq(www_urldecode("Hello%20Dolly"),"Hello Dolly"));
	assert(streq(www_urldecode("%20Hello%20Dolly%20")," Hello Dolly "));
}
void test_form_urldecode()
{
	assert(streq(www_form_urldecode("+Hello+Dolly+")," Hello Dolly "));
	assert(streq(www_form_urldecode("x%5E2%2B%28y%5E2%29+%3D+z+%5E2"),"x^2+(y^2) = z ^2"));

}

void testsuite3()
{
	RUN(test_urldecode);
	RUN(test_form_urldecode);
}


int main()
{
	testsuite1();
	testsuite2();
	testsuite3();
	printf("OK\n");
	return 0;
}
