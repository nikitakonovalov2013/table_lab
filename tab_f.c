#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INT 9
#define HEAD "Ключ №1\tКлюч №2\tЗначение\n"
#define FORM "%i\t%i\t%s"
#define h(x) x%(tab->max_size)

typedef struct {
	int key1;
	int key2;
	int offset;
	int len;
} item;

typedef struct {
	int max_size;
	int t_size;
	int *check;
	item *object;
	FILE *fd;
} table;

static int open_f(table *, char *);
static int create_f(table *, char *, int );
static int tab_load(table *);
static int save(table *);
static int e_add(table *);
static int e_find(table *, int , int);
static int e_delete();
static int e_print();
static int t_print();
static int get_keys(char **, char **, int);
static int get_info(char **);
static int get_int(int *);
static int e_insert(table *, int, int, char *);
static int dialog();

static int e_add(table *tab) {
	
	char *info, *k1, *k2;
	get_keys(&k1, &k2, 0);
	printf("Введите значение элемента: ");
	get_info(&info);
	
	switch (e_insert(tab, atoi(k1), atoi(k2), info)) {
		case 1:
			printf("Данные ключи заняты!\n");
			break;
		case 2:
			printf("Таблица заполнена!\n");
			break;
		case -1:
			return -1;
			break;
	}
	free(info);
	return 0;
}


static int e_insert(table *tab, int key1, int key2, char *str) {
	
	if (e_find(tab, key1, key2) != -1)
		return 1;
	if (tab->t_size >= tab->max_size)
		return 2;

	int n = 0;
	int i = h(key2);
		
	while ((tab->check[i] == 1) && (n < tab->max_size)) {
		i = h(i+1);
		n += 1;
	}
	
	if (n < tab->max_size) {			 
		
		tab->object[i].key1 = key1;
		tab->object[i].key2 = key2;
		tab->object[i].len = strlen(str) + 1;
		
		fseek(tab->fd, 0, SEEK_END);
		tab->object[i].offset = ftell(tab->fd);
		
		if (fwrite(str, sizeof(char), tab->object[i].len, tab->fd) != tab->object[i].len) {
			fprintf(stderr, "Не удалось записать данные.\n");
			return -1;
		}

		tab->check[i] = 1;
		tab->t_size += 1;
		printf("Элемент успешно добавлен.\n");
		return 0;
	} else {
		return 2;
	}
	return 0;
}

		

static int e_find(table *tab, int key1, int key2) {
	
	if (key2 > 0) {
		int i = h(key2);
		int n = 0;
		while ((tab->check[i] == 1 || tab->check[i] == -1) && (n < tab->max_size)) {
			if ((tab->check[i] == 1) && (key2 == tab->object[i].key2))
				return i;
			i = h(i+1);
			n += 1;
		}
	}
		
	if (key1 > 0) {
		for (int j = 0; j < tab->max_size; j++) {
			if ((tab->check[j] == 1 || tab->check[j] == -1) && key1 == tab->object[j].key1) {
				return j;
			}
		}
	}
	return -1;
}


static int get_keys(char ** k1, char ** k2, int num) {
	
	int flag;
	if (num == 1 || num == 0) {
		do {
			flag = 1;
			printf("Введите ключ №1(только натуральные значения):");
			get_info(&(*k1));
			for (int i = 0; i < strlen(*k1); i++) {
				if ((*k1)[0] == '0' || isdigit((*k1)[i]) == 0) {
					flag = 0;
					free(*k1);
					break;
				}
			}
		
			if (strlen(*k1) > 9) {
				flag = 0;
			}
		} while (flag == 0);	
	}
	
	
	if (num == 2 || num == 0) {
		do {
			flag = 1;
			printf("Введите ключ №2(только натуральные значения):");
			get_info(&(*k2));
			for (int i = 0; i < strlen(*k2); i++) {
				if ((*k2)[0] == '0' || (isdigit((*k2)[i])) == 0) {
					flag = 0;
					free(*k2);
					break;
				}
			}
			if (strlen(*k2) > 9) {
				flag = 0;
			}
		} while (flag == 0);
	}
	return 0;
}


static int get_int(int *a) {
	
	char *line;
	get_info(&line);

	if (strlen(line) < 0 || strlen(line) > MAX_INT)
		return -1;
	
	for (int i = 0; i < strlen(line)-1; i++) {
		if ((line[0] == '0') || (isdigit(line[i]) == 0)) {
			return -1;
		}
	}
	
	*a = atoi(line);
	free(line);
	return 0;
}
	
static int get_info(char **g_info) {
	
	int buf = 256;
	int flag = 0;
	char info[256];

	if (((*g_info) = ((char *)malloc(sizeof(char)))) == NULL) {
		fprintf(stderr,"Невозможно выделить память!\n");
		return -1;
	}

	do {
		flag = 0;
		if (((*g_info) = ((char *)realloc((*g_info), buf))) == NULL) {
			fprintf(stderr,"Невозможно выделить память!\n");
			return -1;
		}
		
		fgets(info, 256, stdin);
		strcat(*g_info, info);

		for (int i = 0; i < strlen(*g_info); i++) {
			if ((*g_info)[i] == '\n') {
				flag = 1;
				break;
			} 
		}
		
		buf *= 2;

	} while (flag == 0);
	strtok(*g_info, "\n");
	return 0;
}

static int open_f(table *tab, char *fname) {
	
	if ((tab->fd = fopen(fname,"r+b")) == NULL) {
		//fprintf(stderr, "Невозможно открыть файл%s!\n", fname);
		return 1;
	}

	if (fread(&tab->max_size, sizeof(int), 1, tab->fd) != 1) {
		fprintf(stderr, "Не удалось считать данные из %s.\n", fname);
		return -1;
	}

	if ((tab->object = (item *)calloc(tab->max_size, sizeof(item))) == NULL) {
		fprintf(stderr, "Не удалось выделить память под таблицу.\n");
		return -1;
	}
	
	if ((tab->check = (int *)calloc(tab->max_size, sizeof(int))) == NULL) {
		fprintf(stderr, "Не удалось выделить память.\n");
		return -1;
	
	}

	if (fread(&tab->t_size, sizeof(int), 1, tab->fd) != 1) {
		fprintf(stderr, "Не удалось считать данные из %s.\n", fname);
		return -1;
	}

	if (fread(tab->check, sizeof(int), tab->max_size, tab->fd) != tab->max_size) {
		fprintf(stderr, "Не удалось считать данные из %s.\n", fname);
		return -1;
	}
	
	if (fread(&tab->object, sizeof(item), tab->t_size, tab->fd) != tab->t_size) {
		fprintf(stderr, "Не удалось считать данные из %s.\n", fname);
		return -1;
	}
	return 0;
}

static int create_f(table *tab, char *fname, int size) {

	tab->max_size = size;
	tab->t_size = 0;
		
	if ((tab->fd = fopen(fname,"w+b")) == NULL) {
		fprintf(stderr, "Невозможно создать файл %s\n!", fname);
		return -1;
	}	
	
	if ((tab->object = (item *)calloc(tab->max_size, sizeof(item))) == NULL) {
		fprintf(stderr, "Не удалось выделить память под таблицу.\n");
		return -1;
	}

	if ((tab->check = (int *)calloc(tab->max_size, sizeof(int))) == NULL) {
		fprintf(stderr, "Не удалось выделить память.\n");
		return -1;
	}

	if (fwrite(&tab->max_size, sizeof(int), 1, tab->fd) != 1) {
		fprintf(stderr, "Не удалось записать данные в %s.\n", fname);
		return -1;
	}

	if (fwrite(&tab->t_size, sizeof(int), 1, tab->fd) != 1) {
		fprintf(stderr, "Не удалось записать данные в %s.\n", fname);
		return -1;
	}
	
	if (fwrite(tab->check, sizeof(int), tab->max_size, tab->fd) != tab->max_size) {
		fprintf(stderr, "Не удалось записать данные в %s.\n", fname);
		return -1;
	}
	
	if (fwrite(tab->object, sizeof(item), tab->max_size, tab->fd) != tab->max_size) {
		fprintf(stderr, "Ошибка записи в %s.\n", fname);
		return -1;
	}

	return 0;
}

static int tab_load(table *tab) {

	int max_size = 0;
	char *fname = NULL;
	int flag = 0;

	printf("Введите полное имя файла: ");
	get_info(&fname);
	
	if (fname == NULL)
		return -1;

	switch (open_f(tab, fname)) {
	
			case 1:
				printf("Новый файл.\n");
				do {
					flag = 0;
					printf("Введите максимальный размер таблицы: ");
					flag = get_int(&max_size);
					printf("%i\n",flag);
				} while (flag == -1);
		
				if (create_f(tab, fname, max_size) != 0) {
					return -1;
				}
				break;
			case -1:
				return -1;
				break;
	}
	free(fname);
	return 0;
}	

static int save(table *tab) {

	if (fseek(tab->fd, sizeof(int), SEEK_SET) != 0) {
		fprintf(stderr, "Ошибка при работе с файлом.\n");
		return -1;
	}
	
	if (fwrite(&tab->t_size, sizeof(int), 1, tab->fd) != 1) {
		fprintf(stderr, "Не удалось записать данные.\n");
		return -1;
	}
	
	if (fwrite(tab->check, sizeof(int), tab->max_size, tab->fd) != tab->max_size) {
		fprintf(stderr, "Не удалось записать данные.\n");
		return -1;
	}

	if (fwrite(tab->object, sizeof(item), tab->max_size, tab->fd) != tab->max_size) {
		fprintf(stderr, "Ошибка записи.\n");
		return -1;
	}

	fclose(tab->fd);
	tab->fd = NULL;
	return 0;
}


int main() {

	table tab = {0, 0, NULL, NULL, NULL};
	
	if (tab_load(&tab) != 0) {
		fprintf(stderr, "Ошибка открытия/создания файла\n");
		return -1;
	}
	fclose(tab.fd);
}

