#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include<fcntl.h>
#include<ftw.h>
char input[100];
char arguments[6][20];
int i=0;
void ls(char *directory) //function to implement ls command
{
    int n=0;
    struct dirent **namelist;
    n=scandir(directory,&namelist,NULL,alphasort);
    if(n==-1) //if there is no such directory
    {
        printf("\nNo such directory found!!!\n");
        return;
    }
    printf("\n----------------------------------------------\n");
    printf("\t\t Files in %s",directory);
    printf("\n----------------------------------------------\n");
    n=n-1;
    while (n>2) // Displaying the files in the given directory
    { 
        printf("%s\n",namelist[n]->d_name); 
        free(namelist[n]); 
        n--;
    }
}

void ps(int i)      //function to implement ps command
{
    int id = fork();
    if(id==0)
    {
        printf("\n");
        if(i==1)
            execl("/bin/ps","ps",0);
        else if(i==2)
            execl("/bin/ps","ps",arguments[1],0);  
    }
    else
    {
        wait(NULL);
    }
}

void pmap(int i)      //function to implement pmap command
{
    int id = fork();
    if(id==0)
    {
        printf("\n");
        if(i==2)
            execl("/bin/pmap","pmap",0);
        else if(i==3)
            execl("/bin/pmap","pmap",arguments[1],0);  
    }
    else
    {
        wait(NULL);
    }
}

void wget(int i)          //function to implement wget command
{
    int id = fork();
    if(id==0)
    {
        printf("\n");
        if(i==2)
            execl("/bin/wget","wget",0);
        else if(i==3)
            execl("/bin/wget","wget",arguments[1],0);  
    }
    else
    {
        wait(NULL);
    }
}

void mv(char *arg1 , char *arg2)          //function to implement mv command
{
    char newplace[50];
    if(arg2[0]=='/')				//check to see if input is a path
    {
        strcat(arg2,"/");			//if argument is a Full Path, prepare to mv to end of path.
        strcat(arg2,arg1);
        if(rename(arg1, arg2)== 0)
            printf("Successful\n");
        else
            printf("Error:\nDirectory not found\n");
    }
    else
    {
        DIR *isD;
        isD = opendir(arg2); 				// check if argument is a DIR in CWD
        if(isD==NULL)
        {
            if( rename(arg1,arg2)!= 0)
                printf("Error: File not moved\n");
            else
                printf("Successful\n");
        }
        else
        {
            char *ptrL;
            ptrL = getcwd(newplace, 50);		// get current working directory path 
            strcat(newplace,"/");				
            strcat(newplace,arg2);			// attach mv location to path ptrL
            strcat(newplace,"/");
            strcat(newplace, arg1);				// keep original file name
            if(rename(arg1,ptrL)!=-1)
                printf("Successful\n");
            else
                printf("Error:\nDirectory not found in CWD\n");
            closedir(isD);
        }
    }
}

void _chmod(int i)      //function to implement chmod command
{
    if(i==3)    
    {
        if(chmod(arguments[2],atoi(arguments[1]))!=0)
            printf("!. Incorrect arguments of chmod. \nCorrect usage: chmod permission filename\n           eg: chmod 777 filename\n");
        else
            printf("Permissions changed successfully.\n");
    }
    else
        printf("Incorrect arguments of chmod. \nCorrect usage: chmod permission filename\n           eg: chmod 777 filename\n");
}

int rmFiles(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)        //function to implement rm command
{
	if(remove(pathname)<0)
	{
		perror("Error deleting the file.\n");
		return -1;
	}
	return 0;
}

void _cd(int i)           //function to implement cd command
{
    if(i==1)
    {
        if(chdir("..")!=0)
            printf("Error while changing directory\n");
    }
    else if(i==2)
    {
        if(chdir(arguments[1])!=0)
            printf("Error while changing directory\n");
    }
    else
        printf("Incorrect arguments of cd. \nCorrect usage: cd \n               cd path\n");
}

void background()       // Function to execute a program in the background using &
{   
    char pgm[20];
    int i=0;
    while(input[i]!='&')
    {
        pgm[i]=input[i];
        i++;
    }
    pgm[i]='\0';
    int id=fork();
    if(id==0)
    {
        if(execvp(pgm, NULL)< 0)
        {
            printf("Cannot execute %s\n", pgm);
            exit(0);
        }       
    }
    else{
        return; // Parent process can continue executing while the child runs in the background
    }   
    exit(0); // Child process will exit after completion
}

void CP(int i)
{
    FILE *src,*dest;
    if(i!=3)        //Incorrect number of arguments
    {
        printf("Incorrect arguments of cp. \nCorrect usage: cp source_file destination_file\n");
    }
    else if((src=fopen(arguments[1],"rb"))==NULL)
    {
            printf("Error while opening %s.\n",arguments[1]);
    }
    else if((dest=fopen(arguments[2],"wb"))==NULL)
    {
            printf("Error while opening %s.\n",arguments[2]);
    }
    else 
    {
        printf("Copying\n");
        char ch;
        while((ch = getc(src))!= EOF)       //Copying each characters
        {
            printf("%c\n",ch);
            putc(ch, dest);
        }
        fclose(src);
        fclose(dest);
    }
}
int main()      //main function
{
    while(1)
    {
        printf("\n%s: ",getcwd(arguments[3],100));      //showing the current working directory
        gets(input);
        char *arg=strtok(input," ");
        i=0;
        while(arg!=NULL)        // tokenizing the input
        {
            strcpy(arguments[i],arg);
            arg=strtok(NULL," ");
            i++;
        }
        if(strcmp(arguments[0],"exit")==0)      //To break out of the shell.
            break;
        else if(strcmp(arguments[0],"ps")==0)
        {
            ps(i);
        }
        else if(strcmp(arguments[0],"pmap")==0)
        {
            pmap(i);
        }
        else if(strcmp(arguments[0],"wget")==0)
        {
            wget(i);
        }
        else if(strcmp(arguments[0],"mv")==0)
        {
            if(i!=3)
                printf("Incorrect arguments of mv. \nCorrect usage: mv source target\n               mv source directory\n");
            else
                mv(arguments[1],arguments[2]);
        }
        else if(strcmp(arguments[0],"ls")==0)
        {
            if(i==1)        //when no arguments are specified
                ls(".");
            else if(i==2)
            {
                ls(arguments[1]);
            }        
        }
        else if(strcmp(arguments[0],"chmod")==0)
        {
            _chmod(i);
        }

        else if(strcmp(arguments[0],"pwd")==0 && i==1)
        {
            printf("The current working directory is: %s\n",getcwd(arguments[3],50));
        }
        else if(strcmp(arguments[0],"cd")==0)
        {
            _cd(i);
        }
        else if(strcmp(arguments[0],"cp")==0)
        {
            CP(i);
        }
        else if(strcmp(arguments[0],"mkdir")==0)
        {
            if(i==1)
            {
                printf("Specify a name for the directory\n");
            }
            else if(i==2)
            {
                if(mkdir(arguments[1],0777)!=0)
                    printf("Error while changing directory\n");
                else
                    printf("Successfully created the directory %s\n",arguments[1]);
            }
            else 
            {
               printf("Incorrect arguments of mkdir. \nCorrect usage: mkdir name\n"); 
            }
        }
        else if(strcmp(arguments[0],"rm")==0)
        {
            if(i==2)
            {
                if(remove(arguments[1])!=0)
                    printf("Error while deleting file/directory \n");
                else
                    printf("Successfully deleted the file/directory %s\n",arguments[1]);
            }
            else if(i==3 && strcmp(arguments[1],"-r")==0)
            {
                if(nftw(arguments[2],rmFiles,10,FTW_DEPTH|FTW_MOUNT|FTW_PHYS)<0)
                {
                    perror("Error ntfw");
                    exit(1);
                }
                else
                {
                    printf("Successfully deleted the file/directory %s\n",arguments[1]);
                }
            }
            else 
            {
               printf("Incorrect arguments of rm. \nCorrect usage: rm file_name\n                rm -r dir_name\n"); 
            }
        }
        else if(strcmp(arguments[0],"cat")==0)
        {
            char c;
            int j=1;
            FILE *file;
            if(i>=2)
            {
                while(j<i)
                {
                    if((file=fopen(arguments[j],"r"))!=NULL)
                    {
                        printf("Contents of %s\n",arguments[j]);
                        c = fgetc(file); 
                        while (c != EOF) 
                        { 
                            printf ("%c", c); 
                            c = fgetc(file); 
                        } 
                        fclose(file);
                    }
                    else
                        printf("Error opening file %s.",arguments[j]);
                    printf("\n");
                    j++;
                }
            }
            else 
            {
                printf("Incorrect arguments of cat. \nCorrect usage: cat file_name\n"); 
            }   
        }
        else if(strcmp(arguments[0],"grep")==0)
        {
            if(i==3)
            {
                FILE *file;
                char temp[200];
                if((file=fopen(arguments[2],"r"))!=NULL)
                {
                    while(fscanf(file,"%[^\n]\n",temp)!=EOF)
                    {
                        if(strstr(temp,arguments[1])!=NULL)
                        {
                            printf("%s\n",temp);
                        }
                    }
                    fclose(file);
                }
                else
                    printf("Error opening file %s.",arguments[2]);
            }
            else 
                printf("Incorrect arguments of grep. \nCorrect usage: grep pattern file_name\n"); 
        }
        else if(strcmp(arguments[i-1],"&")==0)
        {
            background();
        }
    }
}
