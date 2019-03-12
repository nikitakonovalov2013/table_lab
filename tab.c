#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 100
#define HEAD "Ключ №1\tКлюч №2\tЗначение\n"
#define FORM "%i\t%i\t%s"
#define h(x) x%MAX_SIZE

typedef struct {
	int key1;
	int key2;
	char *info;
} item;

static int e_add(item *, int *, int *);
static int e_delete(item *, int *, int *, int);
static int e_find(item *, int *, int, int, int);
static int t_print(item *,int *, int );
static int e_print(item *, int *, int, int);
//static int t_random(item *, int *, int *, char *);
static int get_keys(char **, char **, int);

static int get_keys(char ** k1, char ** k2, int num) {
	
	size_t buf = 0;
	int flag;
	if (num == 1 || num == 0) {
	do {
		flag = 1;
		printf("Введите ключ №1(только натуральные значения):");
		if (getline(&(*k1),&buf,stdin) < 0) {
			fprintf(stderr,"Невозможно считать строку!");
			return -1;
		}
		for (int i = 0; i < strlen(*k1)-1; i++) {
			if ((*k1)[0] == '0' || isdigit((*k1)[i]) == 0) {
				flag = 0;
				free(*k1);
				buf = 0;
				break;
			}
		}
	} while (flag == 0);	
	}
	
	buf = 0;
	if (num == 2 || num == 0) {
	do {
		flag = 1;
		printf("Введите ключ №2(только натуральные значения):");
		if (getline(&(*k2),&buf,stdin) < 0) {
			fprintf(stderr,"Невозможно считать строку!");
			return -1;
		}
		for (int i = 0; i < strlen(*k2)-1; i++) {
			if ((*k2)[0] == 0 || isdigit((*k2)[i]) == 0) {
				flag = 0;
				free(*k2);
				buf = 0;
				break;
			}
		}
	} while (flag == 0);
	}
	return 0;
}


	
/*static int t_random(item *table, int *check, int *t_size, char *text) {
	
	for (int i = 0; i < 10; i += 2) {
		table[i].key1 = i;
		table[i].key2 = h(i*10);
		table[i].info = text;
		check[i] = 1;
		*t_size += 1;
		
	}
}*/

static int t_print(item *table, int *check, int t_size) {
	
	if (t_size == 0) {
		printf("Таблица пуста!\n");
		return 0;
	}

	printf(HEAD);
	for (int i = 0; i < MAX_SIZE; i++) {
		if (check[i] == 1)
			printf(FORM, table[i].key1, table[i].key2, table[i].info);
	}
	return 0;
}

static int e_print(item *table, int *check, int t_size, int num) {
	
	int i = 0;
	char *k1, *k2;
	get_keys(&k1, &k2, num);
	
	if (num == 1) {
		
		if ((i = e_find(table, check, atoi(k1), -1, t_size)) > 0) {
			printf(HEAD);
			printf(FORM, table[i].key1, table[i].key2, table[i].info);
			return 0;
		} else {
			printf("Элемент не найден!\n");
			return 0;
		}
	}
	if (num == 2) {
		if ((i = e_find(table, check, -1, atoi(k2), t_size)) > 0) {
			printf(HEAD);
			printf(FORM, table[i].key1, table[i].key2, table[i].info);
			return 0;
		} else {
			printf("Элемент не найден!\n");
			return 0;
		}
	}
		
	return 0;
}

static int e_find(item *table, int *check, int key1, int key2, int t_size) {
	
	if (key2 > 0) {
		int i = h(key2);
		int n = 0;
		while ((check[i] == 1) && (n < MAX_SIZE)) {
			if ((check[i] == 1) && (key2 == table[i].key2))
				return i;
			i = h(i+1);
			n += 1;
		}
	}
		
	if (key1 > 0) {
		for (int j = 0; j < MAX_SIZE; j++) {
			if (check[j] == 1 && key1 == table[j].key1) {
				return j;
			}
		}
	}
	return -1;
}

static int e_add(item *table, int *check, int *t_size) {

	if (*t_size >= MAX_SIZE) {
		printf("Таблица заполнена!\n");
		return -1;
	}
	
	char *k1, *k2;
	get_keys(&k1, &k2, 0);
		
	size_t buf = 0;
	if ((e_find(table, check, atoi(k1), -1, *t_size) < 0) && (e_find(table, check, -1, atoi(k2), *t_size) < 0)) {
		
		char *info;
		printf("Введите значение элемента: ");
		getline(&info, &buf, stdin);
		
		int n = 0;
		int i = h(atoi(k2));
		while (check[i] == 1 && n < MAX_SIZE) {
			i = h(i+1);
			n += 1;
		}
		if (n < MAX_SIZE) {			 
			table[i].key1 = atoi(k1);
			table[i].key2 = atoi(k2);
			asprintf(&table[i].info,"%s",info);
			check[i] = 1;
			*t_size += 1;
			printf("Элемент успешно добавлен.\n");
			free(info);
			return 0;
		}
	} else {
		printf("Данные ключи заняты!\n");
		return -1;
	}

	return 0;
}

static int e_delete(item *table, int *check, int *t_size, int num) {
	
	if (*t_size == 0) {
		printf("Удаление невозможно: таблица пуста!\n");
		return 0;
	}
	
	char *k1, *k2;
	get_keys(&k1, &k2, num);
	int i = 0;
	if (num == 1) {
		if ((i = e_find(table, check, atoi(k1), -1, *t_size)) >= 0) {
			check[i] = 0;
			*t_size -= 1;
			printf("Элемент успешно удален");
			return 0;
		} else {
			printf("Элемент не найден!\n");
			return -1;
		}
	}
	if (num == 2) {
		if ((i = e_find(table, check, -1, atoi(k2), *t_size)) >= 0) {
			check[i] = 0;
			*t_size -= 1;
			printf("Элемент успешно удален");
			return 0;
		} else {
			printf("Элемент не найден!\n");
			return -1;
		}
	}
	return 0;
}	

int main(){
	
	int t_size = 0;
	int check[MAX_SIZE];
	for (int i = 0; i < MAX_SIZE; i++) {
		check[i] = 0;
	}

	item table[MAX_SIZE];
	char *c = (char *)malloc(10 * sizeof(char));
	size_t b = 1;

	printf("Выберите действие:\na - добавить элемент в таблицу;\np - вывести таблицу на экран;\nd - удалить элемент\nk - вывести элемент на экран\nm - показать меню\nq - выйти.\n");
	do {
		do {	
			getline(&c,&b,stdin);
			if (strlen(c) > 2 || strlen(c) < 2)
				printf("Только один символ!\n");
		} while(strlen(c) > 2 || strlen(c) < 2);

		switch (c[0]) {
			case 'a':
				printf("ДОБАВЛЕНИЕ\n");
				e_add(table, check, &t_size);
				break;
			case 'p':
				t_print(table, check, t_size);
				break;
			case 'd':
				printf("УДАЛЕНИЕ\nВыберите пространство ключей (нажмите 1 или 2):\n");
				char *s = (char *)malloc(10 * sizeof(char));
				do {
					getline(&s,&b,stdin);
					if (strlen(s) > 2 || strlen(s) < 2)
						printf("Только один символ!\n");
				} while (strlen(s) > 2 || strlen(s) < 2);
				if (s[0] == '1')
					e_delete(table, check, &t_size, 1);
				if (s[0] == '2')
					e_delete(table, check, &t_size, 2);
				free(s);
				break;
			case 'k':
				printf("ПОИСК\nВыберите пространство ключей (нажмите 1 или 2):\n");
				char *k = (char *)malloc(10 * sizeof(char));
				do {
					getline(&k,&b,stdin);
					if (strlen(k) > 2 || strlen(k) < 2)
						printf("Только один символ!\n");
				} while (strlen(k) > 2 || strlen(s) < 2);
				if (k[0] == '1')
					e_print(table, check, t_size, 1);
				if (k[0] == '2')
					e_print(table, check, t_size, 2);
				free(k);
				break;

			case 'm':
				printf("Выберите действие:\na - добавить элемент в таблицу;\np - вывести таблицу на экран;\nd - удалить элемент\nk - вывести элемент на экран\nm - показать меню\nq - выйти.\n");
				break;
		}
	} while (c[0] != 'q');
	free(c);
/*	for (int i = 0; i < MAX_SIZE; i++) {
		printf("%i\n",check[i]);
	}*/
	return 0;
}
