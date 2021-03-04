#include "stdio.h"
#include "stdlib.h"
#include "dirent.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "fcntl.h"
#include "string.h"
#include "errno.h"
#include "time.h"
 
typedef struct {
  char *name;
  struct spis *next;
} spis;
 
int main(int argc, char **argv) {
    
    clock_t time = clock();
    DIR     *dir;
    spis *files = NULL, *dp_files = NULL, *dp_dp = NULL;
    struct dirent   *f;
    struct stat     stbuf;
    long int    n1 = 0, n2 = 0, i, j;
    char    **directories,
        *dp_string, kl;
    int     nach = 0, kon = 0, kol_files = 0;
    unsigned long long int  size;
    FILE    *f1, *f2, *f_write;
    
    if (argc != 4) {
        printf("Nevernoe kolichestvo argumentotv\n");
        return 1;
    }
    for (i = 0; argv[2][i] != '\0'; i++) 
        n1 = n1 * 10 + argv[2][i] - 48;
      
    for (i = 0; argv[3][i] != '\0'; i++) 
        n2 = n2 * 10 + argv[3][i] - 48;
    
    if (opendir(argv[1]) == NULL) {
        perror("ERROR");
        return 1;
    }
    if (((directories = (char **) malloc(sizeof(char *))) == NULL) || 
       ((directories[0] = (char *) malloc ((strlen(argv[1])+1) * sizeof(char))) == NULL)) {
        perror("ERROR");
        return 1;
    }
    strcpy(directories[0], argv[1]);
    while (nach <= kon) {
        if ((dir = opendir(directories[nach])) == NULL) {
            perror("ERROR");
            printf("%s\n", directories[nach]);
        }
        else {
            while ((f = readdir(dir) ) != NULL) 
            if ((strcmp(f -> d_name,".") != 0) &&
                (strcmp(f -> d_name,"..") != 0))
            {
                
                dp_string = (char *) malloc ((strlen(directories[nach]) + strlen(f -> d_name) + 3) * sizeof(char));
                strcpy(dp_string, directories[nach]);
                strcat(dp_string,"/");
                strcat(dp_string, f -> d_name);
                strcat(dp_string,"\0");
                if (lstat(dp_string, &stbuf) == -1) {           
                    
                    perror("ERROR");
                    printf("%s\n", dp_string);
                    free(dp_string);
                    continue;
                }
                if (stbuf.st_mode & S_IFDIR) {
                    kon++;
                    if (((directories = (char **) realloc(directories,(kon + 1) * sizeof(char *))) == NULL) ||
                        ((directories[kon] = (char *) malloc((strlen(dp_string) + 1) * sizeof(char))) == NULL)) {
                        perror("ERROR");
                        free(dp_string);
                        continue;
                    }
                    strcpy(directories[kon], dp_string);
                }
                
                if (stbuf.st_mode & S_IFREG)  
                    if (stbuf.st_size >= n1 && stbuf.st_size <= n2) {
                        
                        dp_files = (spis *) malloc(sizeof(sizeof(spis *)));
                        dp_files -> name = (char *) malloc((strlen(dp_string) + 1) * sizeof(char));
                        strcpy(dp_files -> name, dp_string);
                        dp_files -> next = files;
                        files = dp_files;
                    }           
                free(dp_string);
            }
        if (closedir(dir) == -1) 
            perror("ERROR");
            
        }
        nach++;
        
        
    }
    if ((f_write = fopen("result.txt","w")) == NULL) {
        perror("ERROR");
        return 1;
    }
    
    
    while (files) {
        if (stat(files -> name, &stbuf) == -1) {
            perror("ERROR");
            printf("%s\n", files -> name);
            files = files -> next;
            continue;
        }
        if ((f1 = fopen(files -> name,"r")) == NULL) {
            perror("ERROR");
            printf("%s\n", files -> name);
            files = files -> next;
            continue;
        }
        size = stbuf.st_size;
        kl = 0;
        dp_dp = files;
        while (dp_dp -> next) {
            dp_files = dp_dp -> next;
            
            if (stat(dp_files -> name, &stbuf) == -1) {
                perror("ERROR");
                printf("%s\n", dp_files -> name);
                dp_dp = dp_dp -> next;
                continue;
            }   
            
            if (size != stbuf.st_size) {
                dp_dp = dp_dp -> next;
                continue;
            }   
            
            if ((f2 = fopen(dp_files-> name,"r")) == NULL) {
                perror("ERROR");
                printf("%s\n", dp_files -> name);
                dp_dp = dp_dp -> next;
                continue;
            }
            while (!feof(f1) && (fgetc(f1) == fgetc(f2)))
                ;
            if (feof(f1)) {
                if (kl == 0) { 
                    fprintf(f_write,"%d\n", size);
                    fprintf(f_write,"%s\n",files -> name);
                    kl = 1;
                }
                fprintf(f_write,"%s\n",dp_files -> name);
                dp_dp -> next = dp_files -> next;
                free(dp_files);
                
            }   else
            dp_dp = dp_dp -> next;
            if (fclose(f2) == EOF) 
                perror("ERROR");        
            fseek(f1,0,SEEK_SET);
        }
        files = files -> next;
        if (kl) 
            fprintf(f_write,"\n");
        if (fclose(f1) == EOF) 
                perror("ERROR");
    }
    
    if (fclose(f_write) == EOF)
        perror("ERROR");
    
    for (i = 0; i<kon; 
        free(directories[i]),
        directories[i] = NULL,
        i++)
        ;
    
    free(directories);
    directories = NULL;
 
    printf("%0.10f\n", (double) (clock() - time) / (double)CLOCKS_PER_SEC);
}
