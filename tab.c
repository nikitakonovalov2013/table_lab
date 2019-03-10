#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 100
#define HEAD "Ключ №1\tКлюч №2\tЗначение\n"
#define FORM "%i\t%i\t%s\n"

typedef struct {
	int key1;
	int key2;
	char *info;
} item;

static int e_add(item *, int *);
static int e_delete();
static int e_find(item *, int, int, int);
static int t_print(item *, int );
static int e_print(item *, int, int, int);
static int t_random(item *, int *, char *);
static int get_int();
//static int get_info(char *);


/*static int get_info(char * g_info) {
	
	int buf = 256;
	int flag = 0;
	
	if (g_info = (char *)malloc(sizeof(char)) == NULL) {
		fprintf(stderr,"Невозможно выделить память");
		return -1;
	}
	
	do {
		if (g_info = (char *)realloc(g_info, buf)) {
			fprintf(stderr,"Невозможно выделить память");
			return -1;
		}
		fgets(g_info,buf,stdin);
		for (int i = 0; i < strlen(g_info); i++) {
			if (g_info[i] == '\n') {
				flag = 1;
			} else {
				buf *= 2;
			}	
		
		}
	} while (flag == 0);
}	*/ 
	
	
static int t_random(item *table, int *t_size, char *text) {
	
	for (int i = 0; i < 99; i++) {
		table[i].key1 = i;
		table[i].key2 = 100+i;
		table[i].info = text;
		*t_size += 1;
	}
}

static int t_print(item *table, int t_size) {
	
	if (t_size == 0) {
		printf("Таблица пуста!\n");
		return 0;
	}

	printf(HEAD);
	for (int i = 0; i < t_size; i++) {
		printf(FORM, table[i].key1, table[i].key2, table[i].info);
	}
	return 0;
}

static int e_print(item *table, int key1, int key2, int t_size) {
	
	int i = 0;
	if (i = e_find(table, key1, key2, t_size) > 0) {
		printf(HEAD);
		printf(FORM, table[i].key1, table[i].key2, table[i].info);
		return 0;
	} else {
		printf("Элемент не найден!\n");
		return 0;
	}
	return 0;
}

static int e_find(item *table, int key1, int key2, int t_size) {
	
	if (key1 < 0) {
		for (int i = 0; i < t_size; i++) {
			if (key2 == table[i].key2) {
				return i;
			}
		}
	}
	
	if (key1 < 0) {
		for (int i = 0; i < t_size; i++) {
			if (key1 == table[i].key1) {
				return i;
			}
		}
	}
	return -1;
}
	
static int e_add(item *table, int *t_size) {


	if (*t_size >= MAX_SIZE) {
		printf("Таблица заполнена!\n");
		return -1;
	}

	size_t buf = 0;
	char *k1, *k2;
	int flag;

	do {
		flag = 1;
		printf("Введите ключ №1(только натуральные значения):");
		getline(&k1,&buf,stdin);
		for (int i = 0; i < strlen(k1)-1; i++) {
			if (k1[0] == '0' || isdigit(k1[i]) == 0) {
				flag = 0;
				free(k1);
				buf = 0;
				break;
			}
		}
	} while (flag == 0);	
	
	buf = 0;
	do {
		flag = 1;
		printf("Введите ключ №2(только натуральные значения):");
		getline(&k2,&buf,stdin);
		for (int i = 0; i < strlen(k2)-1; i++) {
			if (k2[0] == 0 || isdigit(k2[i]) == 0) {
				flag = 0;
				free(k2);
				buf = 0;
				break;
			}
		}
	} while (flag == 0);
	printf("%i", atoi(k1));
	buf = 0;
	if ((e_find(table, atoi(k1), -1, *t_size) < 0) && (e_find(table, -1, atoi(k2), *t_size) < 0)) {
		
		char *info;
		printf("Введите значение элемента: ");
		getline(&info, &buf, stdin);
		
		table[*t_size].key1 = atoi(k1);
		table[*t_size].key2 = atoi(k2);//ПОМЕНЯТЬ
		asprintf(&table[*t_size].info,"%s",info);
		*t_size += 1;
		printf("Элемент успешно добавлен.\n");
		return 0;
	} else {
		printf("Ключи заняты! Возможные свободные ключи:\n");
		printf("Ключ №1: %i\nКлюч №2: %i\n",*t_size+1, *t_size + 101);
		return -1;
	}
	return 0;
}

	
int main(){
	
	int t_size = 0;
	char text[] = "random and !!!";
	char text1[] = "123";
	item table[MAX_SIZE];
	
	t_random(table, &t_size, text);
	e_add(table,&t_size);
	//e_print(table,100,200,t_size);

	//printf("%i\n",t_size);
	//printf("%i %i %s\n", table[5].key1, table[5].key2, table[5].info);

	t_print(table, t_size);
	//e_print(table,-1,105,t_size);
}
