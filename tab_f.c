#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INT 9
#define HEAD "Ключ №1\tКлюч №2\tЗначение\n"
#define FORM "%i\t%i\t%s\n"
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
static int e_delete(table *, int);
static int e_print(table *, int);
static int t_print(table *);
static int get_keys(char **, char **, int);
static int get_info(char **);
static int get_int(int *);
static int e_insert(table *, int, int, char *);
static int dialog(table *);

static int dialog(table *tab) {

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
				if(e_add(tab) == -1) {
					printf("Ошибка при добавлении элемента.\n");
				}
				break;
			case 'p':
				t_print(tab);
				break;
			case 'd':
				printf("УДАЛЕНИЕ\nВыберите пространство ключей (нажмите 1 или 2):\n");
				char *s = (char *)malloc(10 * sizeof(char));
				do {
					getline(&s,&b,stdin);
					if (strlen(s) > 2 || strlen(s) < 2)
						printf("Только один символ!\n");
				} while (strlen(s) > 2 || strlen(s) < 2);
				
				if (s[0] == '1') {
					e_delete(tab, 1);
				}
				else if (s[0] == '2') {
					e_delete(tab, 2);
				} else {
					printf("Неверное значение!\nМЕНЮ:\n");
				}
				free(s);
				break;
			case 'k':
				printf("ПОИСК\nВыберите пространство ключей (нажмите 1 или 2):\n");
				char *k = (char *)malloc(10 * sizeof(char));
				do {
					getline(&k,&b,stdin);
					if (strlen(k) > 2 || strlen(k) < 2)
						printf("Только один символ!\n");
				} while (strlen(k) > 2 || strlen(k) < 2);
				
				if (k[0] == '1') {
					e_print(tab, 1);
				}
				else if (k[0] == '2') {
					e_print(tab, 2);
				} else {
					printf("Неверное значение!\nМЕНЮ:\n");
				}
				free(k);
				break;
					case 'm':
				printf("Выберите действие:\na - добавить элемент в таблицу;\np - вывести таблицу на экран;\nd - удалить элемент\nk - вывести элемент на экран\nm - показать меню\nq - выйти.\n");
				break;
			default:
				if (c[0] != 'q')
					printf("Используйте команды из меню (m - вызвать меню)\n");
				break;
		}
	} while (c[0] != 'q');
	
	free(c);
	return 0;
}
static int e_delete(table *tab, int num) {

	if (tab->t_size == 0) {
		printf("Удаление невозможно. Таблица пуста!\n");
		return -1;
	}
	
	char *k1, *k2;
	get_keys(&k1, &k2, num);
	int i = 0;
	if (num == 1) {
		if ((i = e_find(tab, atoi(k1), -1)) >= 0) {
			tab->check[i] = -1;
			tab->t_size -= 1;
			printf("Элемент успешно удален\n");
			free(k1);
			return 0;
		} else {
			printf("Элемент не найден!\n");
			free(k1);
			return -1;
		}
	}
	if (num == 2) {
		if ((i = e_find(tab, -1, atoi(k2))) >= 0) {
			tab->check[i] = -1;
			tab->t_size -= 1;
			printf("Элемент успешно удален\n");
			free(k2);
			return 0;
		} else {
			printf("Элемент не найден!\n");
			free(k2);
			return -1;
		}
	}
	return 0;
}

	
static int t_print(table *tab) {

	if (tab->t_size == 0) {
		printf("Таблица пуста!\n");
		return -1;
	}

	printf(HEAD);
	char *info;
	for (int i = 0; i < tab->max_size; i++) {
			if (tab->check[i] == 1) {
				
				if ((info = (char *)malloc((tab->object[i].len) * sizeof(char))) == NULL) {
					fprintf(stderr,"Невозможно выделить память!\n");
					return -1;
				}
			
				fseek(tab->fd, tab->object[i].offset, SEEK_SET);

				if (fread(info, sizeof(char), tab->object[i].len, tab->fd) != tab->object[i].len) {
					fprintf(stderr, "Не удалось считать данные.\n");
					return -1;
				}

				printf(FORM, tab->object[i].key1, tab->object[i].key2, info);
				free(info);
			}
	}
	return 0;
}
	


static int e_print(table *tab, int num) {

	if (tab->t_size == 0) {
		printf("Таблица пуста!\n");
		return -1;
	}
	
	int i = 0;
	char *k1, *k2, *info;
	get_keys(&k1, &k2, num);

	if (num == 1) {
		
		if ((i = e_find(tab, atoi(k1), -1)) != -1) {
			
			if ((info = (char *)malloc((tab->object[i].len) * sizeof(char))) == NULL) {
				fprintf(stderr,"Невозможно выделить память!\n");
				return -1;
			}
			
			fseek(tab->fd, tab->object[i].offset, SEEK_SET);

			if (fread(info, sizeof(char), tab->object[i].len, tab->fd) != tab->object[i].len) {
				fprintf(stderr, "Не удалось считать данные.\n");
				return -1;
			}

			printf(HEAD);
			printf(FORM, tab->object[i].key1, tab->object[i].key2, info);
			free(info);
			return 0;
		} else {
			printf("Элемент не найден!\n");
			return 0;
		}
	}
	
	if (num == 2) {
		
		if ((i = e_find(tab, -1, atoi(k1))) != -1) {
			
			if ((info = (char *)malloc((tab->object[i].len) * sizeof(char))) == NULL) {
				fprintf(stderr,"Невозможно выделить память!\n");
				return -1;
			}
			
			fseek(tab->fd, tab->object[i].offset, SEEK_SET);

			if (fread(info, sizeof(char), tab->object[i].len, tab->fd) != tab->object[i].len) {
				fprintf(stderr, "Не удалось считать данные.\n");
				return -1;
			}

			printf(HEAD);
			printf(FORM, tab->object[i].key1, tab->object[i].key2, info);
			free(info);
			return 0;
		} else {
			printf("Элемент не найден!\n");
			return 0;
		}
	}
			
	return 0;
}



static int e_add(table *tab) {
	
	char *info, *k1, *k2;
	get_keys(&k1, &k2, 0);
	printf("Введите значение элемента: ");
	get_info(&info);
	int key1 = atoi(k1);
	int key2 = atoi(k2);
	
	switch (e_insert(tab, key1, key2, info)) {
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
	
	for (int i = 0; i < strlen(line); i++) {
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

	if (((*g_info) = ((char *)malloc(buf*sizeof(char)))) == NULL) { 
		fprintf(stderr, "Невозможно выделить память!\n"); 
		return -1; 
	} 
	fgets(info, buf, stdin); 
	for (int i = 0; i < strlen(info); i++) { 
		if ((info)[i] == '\n') { 
			flag = 1; 
			break; 
		} 
	} 
	strcpy(*g_info, info); 
	while (flag == 0) { 
		buf += 256; 
		if (((*g_info) = ((char *)realloc((*g_info), buf))) == NULL) { 
			fprintf(stderr, "Невозможно выделить память!\n"); 
			return -1; 
		} 

		fgets(info,256, stdin); 
		for (int i = 0; i < strlen(info); i++) { 
			if ((info)[i] == '\n') { 
				flag = 1; 
				break; 
			} 
		} 
		strcat(*g_info, info); 
	} 
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
	
	if (fread(tab->object, sizeof(item), tab->max_size, tab->fd) != tab->max_size) {
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

	dialog(&tab);
	save(&tab);
	free(tab.check);
	free(tab.object);
	
	return 0;
}

