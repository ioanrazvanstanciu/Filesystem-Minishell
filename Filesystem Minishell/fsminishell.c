#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "file.h"

//elimina enter-ul de la finalul sirul citit
char *eliminare_enter(char *sir)
{
	int size;

	size = strlen(sir);

	if (size > 0 && sir[size - 1] == '\n')
		sir[size - 1] = '\0';
	return sir;
}

//creeaza si initializeaza filesystem
void create_dir(Directory *dir, Directory *parinte, char *nume_dir)
{
	dir->name = NULL;
	dir->name = nume_dir;
	dir->parentDir = parinte;
	dir->files = NULL;
	dir->directories = NULL;
}

//gaseste ordinea buna a fisierelor
int fis_order(Node_file *a, Node_file *b)
{
	return strcmp(a->data.name, b->data.name) < 0;
}

//insereaza fisierul in locul potrivit
int insert_file(Node_file **head, Node_file *val)
{
	//lista goala, doar adauga
	if (*head == NULL) {
		*head = val;
		return 0;
	}

	Node_file *first = *head;
	Node_file *second = (*head)->next;

	while (second != NULL && fis_order(first,
			val) && fis_order(second, val)) {
		first = first->next;
		second = second->next;
	}
	//insereaza la inceput
	if (fis_order(val, first) && *head == first) {
		*head = val;
		val->next = first;
	return 0;
	}
	//cazul general, dar si introducerea intre alte fisiere
	val->next = second;
	first->next = val;
	return 0;
}

//realizarea functiei touch
Node_file *touch_function(Directory *dir, char *file_name, char *data)
{
	Node_file *new_file = (Node_file *)malloc(sizeof(Node_file));
	int dimensiune = strlen(data);

	new_file->data.name  = (char *)malloc(50 * sizeof(char));
	strcpy(new_file->data.name, file_name);
	new_file->data.size = dimensiune;
	new_file->data.data = (char *)malloc(25 * sizeof(char));
	strcpy(new_file->data.data, data);
	new_file->data.dir = dir;

	new_file->next = NULL;

	insert_file(&(dir->files), new_file);

	return new_file;
}

//gaseste ordinea buna a directoarelor
int dir_order(Node_directory *a, Node_directory *b)
{
	return strcmp(a->data.name, b->data.name) < 0;
}

//insereaza directorul in locul potrivit
int insert_directory(Node_directory **head, Node_directory *val)
{
	//lista goala, doar adauga
	if	(*head == NULL) {
		*head = val;
		return 0;
	}

	Node_directory *first = *head;
	Node_directory *second = (*head)->next;

	while (second != NULL && dir_order(first,
			val) && dir_order(second, val)) {
		first = first->next;
		second = second->next;
	}
	//insereaza la inceput
	if (dir_order(val, first) && *head == first) {
		*head = val;
		val->next = first;
	return 0;
	}
	//cazul general, dar si introducerea intre alte directoare
	val->next = second;
	first->next = val;
	return 0;
}

//realizarea functiei mkdir
Node_directory *mkdir_function(Directory *dir, char *directory_name)
{
	Node_directory *new_directory;

	new_directory = (Node_directory *)malloc(sizeof(Node_directory));

	new_directory->data.name  = (char *)malloc(50 * sizeof(char));
	strcpy(new_directory->data.name, directory_name);
	new_directory->data.parentDir = dir;
	new_directory->data.files = NULL;
	new_directory->data.directories = NULL;

	new_directory->next = NULL;

	insert_directory(&(dir->directories), new_directory);

	return new_directory;
}

/*
 *realizarea functiei ls, in care tin cont de fisierele introduse
 *pentru a afisa directoarelor cu o buna spatiere
 */
void ls_function(Node_file *files, Node_directory *directories)
{
	Node_file *aux_files = files;
	Node_directory *aux_directories = directories;
	int i = 0, j = 1;

	while (aux_files != NULL) {
		if (++i == 1)
			printf("%s", aux_files->data.name);
		else
			printf(" %s", aux_files->data.name);
		aux_files = aux_files->next;
	}

	while (aux_directories != NULL) {
		if (j == 1)
			if (i == 0)
				printf("%s", aux_directories->data.name);
			else
				printf(" %s", aux_directories->data.name);
		else
			printf(" %s", aux_directories->data.name);
		aux_directories = aux_directories->next;
		j++;
	}
	printf("\n");
}

//implementarea functiei cd
Node_directory *cd_function(Directory *dir, char *directory_used)
{
	Node_directory *aux_directories = dir->directories;

	while (aux_directories != NULL) {
		if (strcmp(aux_directories->data.name, directory_used) == 0)
			return aux_directories;
		aux_directories = aux_directories->next;
	}

	printf("Cannot move to '%s': No such directory!\n", directory_used);

	return NULL;
}

/*
 *functia pwd a fost realizata recursiv, fiind considerate
 *toate cazurile pentru afisare
 */
void pwd_afisare(Directory *dir, int print_sep)
{
	if (dir->parentDir == NULL) {
		printf("/");
	if (print_sep == 0)
		printf("\n");
	return;
	}
	pwd_afisare(dir->parentDir, 1);
	printf("%s", dir->name);
	if (print_sep == 1)
		printf("/");
	else
		printf("\n");
}

//se apeleaza functia pwd de afisare
void pwd_function(Directory *dir)
{
	pwd_afisare(dir, 0);
}

int main(void)
{
	//directorul de lucru
	Directory fs;
	//directorul in care ne aflam
	Directory *current = &fs;

	//un nod din lista de directoare utilizat pentru functia 'cd'
	Node_directory *new_directory;

	new_directory = (Node_directory *)malloc(sizeof(Node_directory));

	//sirul citit pentru a realiza comenzi
	char *sir;

	sir = (char *)malloc(100 * sizeof(char));

	//sirul in care se retine comanda
	char *comanda;

	comanda = (char *)malloc(50 * sizeof(char));

	//date pentru fisiere
	char *file_name, *data;

	file_name = (char *)malloc(25 * sizeof(char));
	data = (char *)malloc(25 * sizeof(char));

	//numele directorului ce se introduce in lista de directoare
	char *directory_name;

	directory_name = (char *)malloc(50 * sizeof(char));

	//numele directorului in care urmeaza sa intram cu comanda 'cd'
	char *directory_used;

	directory_used = (char *)malloc(50 * sizeof(char));

/*
 *citirea comenzii 'create fs' care creeaza
 *radacina sistemului de fisiere
 */
	fgets(sir, 100, stdin);
	sir = eliminare_enter(sir);

	//initializarea sistemului de fisiere
	create_dir(&fs, NULL, "/");

	while (strcmp(sir, "delete fs") != 0) {
		fgets(sir, 50, stdin);
		sir = eliminare_enter(sir);
		if (strstr(sir, "touch") != NULL) {
			comanda = strtok(sir, " ");
			file_name = strtok(NULL, " ");
			data = strtok(NULL, " ");
			touch_function(current, file_name, data);
		}
		if (strstr(sir, "mkdir") != NULL) {
			comanda = strtok(sir, " ");
			directory_name = strtok(NULL, " ");
			mkdir_function(current, directory_name);
		}
		if (strstr(sir, "ls") != NULL)
			ls_function(current->files, current->directories);
		if (strstr(sir, "cd") != NULL) {
			comanda = strtok(sir, " ");
			directory_used = strtok(NULL, " ");
			if (strcmp(directory_used, "..") == 0) {
				if (current->parentDir)
					current = current->parentDir;
			} else {
				new_directory = cd_function(current,
					directory_used);
				if (new_directory)
					current = &(new_directory->data);
			}
		}
		if (strstr(sir, "pwd") != NULL)
			pwd_function(current);
	}

	return 0;
}
