#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <errno.h>

#define GREEN "\x1b[92m"
#define BLUE "\x1b[94m"
#define CYAN "\x1b[96m"
#define WHITE "\033[0m"
#define RED "\033[0;31m"
#define PINK "\033[38;5;213m"
#define PURPLE "\033[0;35m"

char* folderPath;

void dbxcli_rm(const char* path) {
	int pid = fork();
	if (pid == 0) {
		execlp("dbxcli", "dbxcli", "rm","-f", path, NULL);
	} else {
		wait(NULL);
	} 
}

void dbxcli_get(const char* remote, const char* local) {
	int pid = fork();
	if (pid == 0) {
		execlp("dbxcli", "dbxcli", "get", remote, local, NULL);
	} else {
		wait(NULL);
	}
}

void dbxcli_put(const char* local, const char* remote) {
	int pid = fork();
	if (pid == 0) {
		execlp("dbxcli", "dbxcli", "put", local, remote, NULL);
	} else {
		wait(NULL);
	}
}

void dbxcli_mv(const char* src, const char* dst) {
	int pid = fork();
	if (pid == 0) {
		execlp("dbxcli", "dbxcli", "mv", src, dst, NULL);
	} else {
		wait(NULL);
	}
}

void dbxcli_cp(const char* src, const char* dst) {
	int pid = fork();
	if (pid == 0) {
		execlp("dbxcli", "dbxcli", "cp", src, dst, NULL);
	} else {
		wait(NULL);
	}
}

void dbxcli_ls(char* path) {
	pid_t pid = fork();
	if (pid == 0) {
		execlp("dbxcli", "dbxcli", "ls", path, NULL);
	} else {
		wait(NULL);
		printf("\n");
	}
}

void dbxcli_mkdir(char* dbx_path) {
	pid_t pid = fork();
	if (pid == 0) {
		// child process
		execlp("dbxcli", "dbxcli", "mkdir", dbx_path, NULL);
	} else {
		wait(NULL);
	}
}

void dbxcli_upload_folder(const char* local_path, const char* remote_path) {
	DIR* dir;
	struct dirent* entry;
	char new_local_path[1024];
	char new_remote_path[1024];

	struct stat file_info;
	stat(local_path, &file_info);
	
	// Calea data este pentru un fisier
	if (S_ISREG(file_info.st_mode)) {
		dbxcli_put(local_path, remote_path);
		return;
	} 

	// Verificam daca exista folderul local dat ca parametru
	if (!(dir = opendir(local_path))){
		printf("No such file or directory!\n");
		return;
	}

	// Calea data este pentru un folder
	
	// Cat timp folderul mai are elemente
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
		// Determinam calea noua atat pentru folderul local, cat si pentru cel remote 
		sprintf(new_local_path, "%s/%s", local_path, entry->d_name);
		sprintf(new_remote_path, "%s/%s", remote_path, entry->d_name);
	 
	 	// Daca am gasit un folder, apelam recursiv pentru acesta
		if (entry->d_type == DT_DIR) {
			if (strcmp(entry->d_name, ".") == 0)
				continue;

			// Creem un folder in care vom incarca fisierele corespunzatoare
			dbxcli_mkdir(new_remote_path);
			dbxcli_upload_folder(new_local_path, new_remote_path);
		} else {
			// Avem un simplu fisier pe care il incarcam in folderul remote
			dbxcli_put(new_local_path, new_remote_path);
		}
	}
	closedir(dir);
}

int exists(const char* path) {
	pid_t pid = fork();
	if (pid == 0) {
		// child process
		FILE *fp;
		fp = freopen("output2.txt", "w", stdout);	
		
		char *temp = (char*) malloc(sizeof(char) * 1024);
		strcpy(temp, path);
		
		for (int j = strlen(temp) - 1; j >= 0; j -= 1) {
			if (temp[j] == '/') {
				temp[j] = '\0';
				break;
			}
		}
			
		execlp("dbxcli", "dbxcli", "ls", temp, NULL);
		fclose(fp);
		free(temp);
	} else {
		wait(NULL);
		
		char *string;
		char *aux, *copy = (char*) malloc(sizeof(char) * 1024);
		
		FILE *fp;
		long numbytes;
		fp = fopen("output2.txt", "r");

		if (fp)
		{  
			// obtinem numarul de bytes
			fseek(fp, 0L, SEEK_END);
			numbytes = ftell(fp);
			 
			// mergem la inceputul fisierului
			fseek(fp, 0L, SEEK_SET);	
			 
			// alocam memorie necesara 
			string = (char*)calloc(numbytes, sizeof(char));	
			 
			// memorie insuficienta
			if(string == NULL)
			    return 0;
			 
			// copiem string-ul 
			fread(string, sizeof(char), numbytes, fp);
				
			strcpy(copy, folderPath);
				
			int find = 0;	
			aux = strtok(string, " \t\n");
			while (aux != NULL) {
				if (strcmp(path, aux) == 0) {
					find = 1;
				}
				aux = strtok(NULL, " \t\n");
			}
			
			if (find == 0) {
				return 0;
			} else {
				return 1;
			}
		} else {
			printf("An error occurred while opening the file.\n");
		}
		fclose(fp);
		free(string);
		free(aux);
		free(copy);	
	}
}

void verif(const char* dbx_path, const char* local_path) {
	pid_t pid = fork();
	if (pid == 0) {
		// child process
		FILE *fp;
		fp = freopen("output2.txt", "w", stdout);		
		execlp("dbxcli", "dbxcli", "ls", dbx_path, NULL);
		fclose(fp);
	} else {
		wait(NULL);
		char *string;
		char *copy = (char*) malloc(sizeof(char) * 1024);
		char *aux = (char*) malloc(sizeof(char) * 1024);
		//char aux[1024];
		
		FILE *fp;
		long numbytes;
		fp = fopen("output2.txt", "r");

		if (fp)
		{  
			// obtinem numarul de bytes
			fseek(fp, 0L, SEEK_END);
			numbytes = ftell(fp);
			 
			// mergem la inceputul fisierului
			fseek(fp, 0L, SEEK_SET);	
			 
			// alocam memorie necesara 
			string = (char*)calloc(numbytes, sizeof(char));	
			 
			// memorie insuficienta
			if(string == NULL)
			    return;
			 
			// copiem string-ul 
			fread(string, sizeof(char), numbytes, fp);
			string[strlen(string)] = '\0';
			
			fclose(fp);
				
			int find = 0;	
			char *saveptr;	
			aux = strtok_r(string, " \t\n\a\0x3\0xBF", &saveptr);
			
			char copy1[1024];
			while (aux != NULL) {
				printf("%s\n", aux);
				find = 0;
				for (int i = strlen(aux) - 1; i >= 0 ; i--) {
					if (aux[i] == '.') {
						// am gasit un fisier
						find = 1;
						dbxcli_get(aux, local_path);
						break;
					}	
				}
				// avem folder
				if (find == 0) {
					char sir[1024];
					char newLocalPath[1024];
					strcpy(newLocalPath, local_path);
					for (int index = strlen(aux) - 1; index >= 0; index --)
						if (aux[index] == '/') {
							strcpy(sir, aux + index);
							break;
						}

					strcat(newLocalPath, sir);
					int status = mkdir(newLocalPath, S_IRWXU | S_IRWXG | S_IRWXO);
					if (status == 0) {
						strcpy(copy1, folderPath);
						strcat(copy1, aux);
					} else {
						printf("Failed to create directory %s\n", newLocalPath);
					}
					verif(aux, newLocalPath);
				}
				aux = strtok_r(NULL, " \t\n\a\03\0BF", &saveptr);
			}
			free(aux);
			free(copy);
			free(string);
		} else {
			printf("An error occurred while opening the file.\n");
		}
	}		
}

void dbxcli_cd(const char* path) {
	if (path == NULL){
		strcpy(folderPath, "");
		return;
	}
	if (strcmp(path, "..") == 0) {
		int i;
		for (i = strlen(folderPath) - 1; i >= 0; i -= 1) {
			if (folderPath[i] == '/') {
				folderPath[i] = '\0';
				break;
			}
		}
		return;
	}
	char *fullPath = (char*) malloc(sizeof(char) * 100);
	pid_t pid = fork();
	if (pid == 0) {
		// child process
		FILE *fp;
		fp = freopen("output2.txt", "w", stdout);		
		execlp("dbxcli", "dbxcli", "ls", folderPath, NULL);
		fclose(fp);
	} else {
		wait(NULL);
		
		char *string;
		char *aux, *copy = (char*) malloc(sizeof(char) * 1024);
		
		FILE *fp;
		long numbytes;
		fp = fopen("output2.txt", "r");

		if (fp)
		{  
			// obtinem numarul de bytes
			fseek(fp, 0L, SEEK_END);
			numbytes = ftell(fp);
			 
			// mergem la inceputul fisierului
			fseek(fp, 0L, SEEK_SET);	
			 
			// alocam memorie necesara 
			string = (char*)calloc(numbytes, sizeof(char));	
			 
			// memorie insuficienta
			if(string == NULL)
			    return;
			 
			// copiem string-ul 
			fread(string, sizeof(char), numbytes, fp);
				
			strcpy(copy, folderPath);
			if (path[0] != '/') {
				strcat(copy, "/");
				strcat(copy, path);
			} else {
				strcat(copy, path);
			}
				
			int find = 0;	
			aux = strtok(string, " \t\n");
			while (aux != NULL) {
				if (strcmp(copy, aux) == 0) {
					find = 1;
					if (path[0] != '/') {
						strcat(folderPath, "/");
						strcat(folderPath, path);
					} else {
						strcat(folderPath, path);
					}
				}
				aux = strtok(NULL, " \t\n");
			}
			
			if (find == 0) {
				printf("No directory found!\n");
			}
		} else {
			printf("An error occurred while opening the file.\n");
		}
		fclose(fp);
		
		free(aux);
		free(copy);
	}	
	free(fullPath);
}

void invalidArg() {
	printf("%s%s", RED, "Invalid number of arguments!\n");
}

void help() {
	printf("%s%s" ,PINK, "\nAvailable commands: \n");
	printf("\n");
	printf("%s%s" ,PINK,"cp          ");
	printf("%s%s", WHITE, "Copy files\n");
	printf("%s%s" ,PINK,"get         ");
	printf("%s%s", WHITE, "Download a file\n");
	printf("%s%s" ,PINK,"ls          ");
	printf("%s%s", WHITE, "List files\n");
	printf("%s%s" ,PINK,"mkdir       ");
	printf("%s%s", WHITE, "Create a new directory\n");
	printf("%s%s" ,PINK,"mv          ");
	printf("%s%s", WHITE, "Move files\n");
	printf("%s%s" ,PINK,"put         ");
	printf("%s%s", WHITE, "Upload files\n");
	printf("%s%s" ,PINK,"rm          ");
	printf("%s%s", WHITE, "Remove files\n");
	printf("%s%s" ,PINK,"cd          ");
	printf("%s%s", WHITE, "Change directory\n");
	printf("%s%s" ,PINK,"quit/q      ");
	printf("%s%s", WHITE, "Exit the shell\n");
	printf("\n");
}

int main(int argc, char* argv[]) {
	char input[256];
	char copy1[256], copy2[256];
	folderPath = (char*) malloc(sizeof(char) * 100);
	strcpy(folderPath, "");
	printf(PINK);
	printf("┌────────────────────────────────────────────────────────┐\n");
	printf("%s%s", PINK, "│ Welcome! Use help for more information about a command.│\n");
	printf("│              Developed by Vali and Daria.              │\n"); 
	printf("└────────────────────────────────────────────────────────┘");
	printf("\n");
	printf("\n");
			
	while (1) {
		printf("%s%s%s%s%s", CYAN, "wdbx:", GREEN, folderPath, "$ ");
		printf(WHITE);
		if(fgets(input, 256, stdin) == NULL) { 
			printf("%s%s", PINK, "\nHave a nice day! :)\n");
			printf(WHITE);
			fflush(stdin);
			return 0;
		} else {
			input[strcspn(input, "\n")] = 0; // sterge endline-ul

			char* command = strtok(input, " ");
			if (command == NULL) {
				continue;
			} else if (strcmp(command, "put") == 0) { 
				char* local_path = strtok(NULL, " ");
				char* dbx_path = strtok(NULL, " ");
				
				if (local_path == NULL || dbx_path == NULL) {
					invalidArg();
				} else {
					char *aux = (char*) malloc(sizeof(char) * 100);
					
					if (dbx_path[0] == '/') {	
						dbxcli_upload_folder(local_path, dbx_path);	
					} else {
						strcpy(aux, folderPath);
						strcat(aux, "/");
						strcat(aux, dbx_path);
						dbxcli_upload_folder(local_path, aux);
					}
					free(aux);
				}
			} else if (strcmp(command, "get") == 0) { 
				char* dbx_remote_path = strtok(NULL, " ");
				char* dbx_local_path = strtok(NULL, " ");
				
				if (dbx_remote_path == NULL || dbx_local_path == NULL) {
					invalidArg();
				} else {
					char local[1024];
					strcpy(local, dbx_local_path);
					char *aux = (char*) malloc(sizeof(char) * 100);
					
					if (dbx_remote_path[0] != '/') {
						strcpy(aux, folderPath);
						strcat(aux, "/");
						strcat(aux, dbx_remote_path);
						strcpy(dbx_remote_path, aux);
					}
					
					if (exists(dbx_remote_path)) {
						int find = 0;
						for (int i = strlen(dbx_remote_path) - 1; i >= 0 ; i--) {
							if (dbx_remote_path[i] == '.') {
								find = 1;
								dbxcli_get(dbx_remote_path, local);
								break;
							}	
						}
						// avem folder
						if (find == 0) {
							mkdir(local, S_IRWXU | S_IRWXG | S_IRWXO);
							verif(dbx_remote_path, local);
						}
					} else {
						printf("File or directory not found!\n");
					}
					free(aux);
				}
			} else if (strcmp(command, "ls") == 0) { 
				char* dbx_path = strtok(NULL, " ");
				
				if (dbx_path != NULL) {
					if (dbx_path[0] == '/'){
					dbxcli_ls(dbx_path);
					} else {
						char copie[1024];
						strcpy(copie, folderPath);
						strcat(copie, "/");
						strcat(copie, dbx_path);
						dbxcli_ls(copie);
					}
				} else {
					dbxcli_ls(folderPath);
				}	
			} else if (strcmp(command, "mv") == 0) { 
				char dbx_path_src_aux[256];
				char dbx_path_dst_aux[256];
				char* dbx_path_src = strtok(NULL, " ");
				char* dbx_path_dst = strtok(NULL, " ");
				
				if (dbx_path_src == NULL || dbx_path_dst == NULL) {
					invalidArg();
				} else {
					strcpy(dbx_path_src_aux, dbx_path_src);
					strcpy(dbx_path_dst_aux, dbx_path_dst);
					if (strstr(dbx_path_src, "/") == NULL) {
						strcpy(copy1, folderPath);
						strcat(copy1, "/");
						strcat(copy1, dbx_path_src_aux);
						strcpy(dbx_path_src_aux, copy1);
					}
					if (strstr(dbx_path_dst, "/") == NULL) {
						strcpy(copy2, folderPath);
						strcat(copy2, "/");
						strcat(copy2, dbx_path_dst_aux);
						strcpy(dbx_path_dst_aux, copy2);
					}
					dbxcli_mv(dbx_path_src_aux, dbx_path_dst_aux);
				}
			} else if (strcmp(command, "mkdir") == 0) { 
				char* dbx_path = strtok(NULL, " ");
				
				if (dbx_path == NULL) {
					invalidArg();
				} else {
					char dbx_path_aux[256];
					strcpy(dbx_path_aux, dbx_path);
					
					if (strstr(dbx_path, "/") == NULL) {
						strcpy(copy1, folderPath);
						strcat(copy1, "/");
						strcat(copy1, dbx_path_aux);
						strcpy(dbx_path_aux, copy1);
					}
					
					dbxcli_mkdir(dbx_path_aux);	
				}
			} else if (strcmp(command, "cp") == 0) { 
				char dbx_path_src_aux[256];
				char dbx_path_dst_aux[256];				
								
				char* dbx_path_src = strtok(NULL, " ");
				char* dbx_path_dst = strtok(NULL, " ");
				
				if (dbx_path_src == NULL || dbx_path_dst == NULL) {
					invalidArg();
				} else {
					strcpy(dbx_path_src_aux, dbx_path_src);
					strcpy(dbx_path_dst_aux, dbx_path_dst);
					if (strstr(dbx_path_src, "/") == NULL && strlen(folderPath) > 0) {
						strcpy(copy1, folderPath);
						strcat(copy1, "/");
						strcat(copy1, dbx_path_src_aux);
						strcpy(dbx_path_src_aux, copy1);
					}
					if (strstr(dbx_path_dst, "/") == NULL && strlen(folderPath) > 0) {
						strcpy(copy2, folderPath);
						strcat(copy2, "/");
						strcat(copy2, dbx_path_dst_aux);
						strcpy(dbx_path_dst_aux, copy2);
					}
					dbxcli_cp(dbx_path_src_aux, dbx_path_dst_aux);
				}
			} else if (strcmp(command, "rm") == 0) { 
				char* dbx_path = strtok(NULL, " ");
				
				if (dbx_path == NULL) {
					invalidArg();
				} else {
					char dbx_path_aux[256];
					strcpy(dbx_path_aux, dbx_path);
					
					if (strstr(dbx_path, "/") == NULL) {
						strcpy(copy1, folderPath);
						strcat(copy1, "/");
						strcat(copy1, dbx_path_aux);
						strcpy(dbx_path_aux, copy1);
					}
									
					dbxcli_rm(dbx_path_aux);
				}
			} else if (strcmp(command, "cd") == 0) { 
				char* dbx_path = strtok(NULL, " ");
				dbxcli_cd(dbx_path);
			} else if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) { 
				printf("%s%s", PINK, "Have a nice day! :)\n");
				printf(WHITE);
				break;
			} else if (strcmp(command, "help") == 0) { 
				help();
			}else {
				printf("%s%s", RED, "Invalid command! Type help for available commands.\n");
			}
		}	
	}
	free(folderPath);
	return 0;
}
