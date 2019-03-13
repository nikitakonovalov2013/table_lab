#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INT 9
#define HEAD "Ключ №1\tКлюч №2\tЗначение\n"
#define FORM "%i\t%i\t%s"
#define h(x) x%MAX_SIZE

typedef struct {
	int key1;
	int key2;
	int offset;
	int len;
} item;

typedef struct {
	int max_size;
	int t_size;
	item *object;
	FILE *fd;
} table;

static int open_f(table *, char *);
static int create_f(table *, char *, int );
static int tab_load(table *tab);
static int save(table *tab);
static int e_add();
static int e_find();
static int e_delete();
static int e_print();
static int t_print();
static int get_keys();
static int get_info(char **);
static int get_int(int *);

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
		
		fgets((*g_info), buf, stdin);

		for (int i = 0; i < strlen(*g_info); i++) {
			if ((*g_info)[i] == '\n') {
				flag = 1;
				break;
			} 
		}
		
		if (flag == 0)
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
	
	if (fread(&tab->t_size, sizeof(int), 1, tab->fd) != 1) {
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

	if (fwrite(&tab->max_size, sizeof(int), 1, tab->fd) != 1) {
		fprintf(stderr, "Не удалось записать данные в %s.\n", fname);
		return -1;
	}

	if (fwrite(&tab->t_size, sizeof(int), 1, tab->fd) != 1) {
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
	
	if (fwrite(tab->object, sizeof(item), tab->max_size, tab->fd) != tab->max_size) {
		fprintf(stderr, "Ошибка записи в %s.\n", fname);
		return -1;
	}

	fclose(tab->fd);
	tab->fd = NULL;
	return 0;
}


int main() {

	table tab = {0, 0, NULL, NULL};
	if (tab_load(&tab) != 0) {
		fprintf(stderr, "Ошибка открытия/создания файла\n");
		return -1;
	}
	fclose(tab.fd);
}

