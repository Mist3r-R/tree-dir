#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define NORMAL_COLOR  	"\x1B[0m"
#define GREEN  			"\x1B[32m"
#define BLUE  			"\x1B[34m"
#define RED     		"\x1b[31m"
#define YELLOW  		"\x1b[33m"
#define MAGENTA 		"\x1b[35m"
#define CYAN		    "\x1b[36m"
#define WHITE		    "\x1b[37m"


void help(char* arg);
int parseArgs(char* arg1, char* arg2);
void print_tree(char * path, char * line);
int colored = 0;
int graphics = 0;
char* concatination(const char* fst, const char* sst);
char* ftypeColor(struct stat *sb);
size_t FilesCount(DIR * dir);
char* getLine(unsigned int type);

int main(int argc, char** argv)
{
	if (argc < 2)
    {
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1],"-h") == 0)
    {
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

	if(argc == 3)
	{
		if (strcmp(argv[2],"-c") == 0)
			colored = 1;

		if (strcmp(argv[2], "-g") == 0)
			graphics = 1;


		if (colored == 0 && graphics == 0)
		{
            help(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (argc == 4)
	{
		if (parseArgs(argv[2],argv[3]) == EXIT_FAILURE)
		{
			help(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		if (!isatty (1) || !colored) {
			fprintf(stdout, "Current working dir: %s\n", cwd);
		}
		else {
			fprintf(stdout, "Current working dir: "GREEN"%s\n"NORMAL_COLOR, cwd);
		}
	}
	printf("\nBuilding tree of :  %s\n\n.\n", argv[1]);
	print_tree(argv[1],"");
	return EXIT_SUCCESS;
}

void help(char * arg)
{
    fprintf(stderr, "Usage:\n %s [PATH] [OPTIONS]\n\nPermitted options:\n\t-c (for color output)"
            "\n\t-g (for graphics)\nUse -h for help\n", arg);
}

int parseArgs(char* arg1, char* arg2)
{
    if (strcmp(arg1,"-c") == 0)
        colored = 1;
    else
        if (strcmp(arg1, "-g") == 0)
            graphics = 1;
        else
            return EXIT_FAILURE;
    if (strcmp(arg2, "-g") == 0 && !graphics)
        graphics = 1;
    else
        if (strcmp(arg2,"-c") == 0 && !colored)
            colored = 1;
        else
            return EXIT_FAILURE;

    if (colored != 1 || graphics != 1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

char* concatination(const char* fst, const char* sst)
{
	char* name_with_extension;
	name_with_extension = malloc(strlen(fst) + 1 +
								 strlen(sst)); /* make space for the new string (should check the return value ...) */
	strcpy(name_with_extension, fst); /* copy name into the new var */
	strcat(name_with_extension, sst); /* add the extension */

	return name_with_extension;
}


void print_tree(char * path, char * line)
{
	DIR * d = opendir(path); // open the path
	if ( !d ) // if was not able return
	{
		printf("\nFailed to open ..!!");
		printf(" : %s", path);
		return;
	}
	size_t count = FilesCount(d);
	struct dirent * dir; // for the directory entries
	while ((dir = readdir(d)) != NULL) // if we were able to read something from the directory
	{
		if(dir-> d_type != DT_DIR) // if the type is not directory just print it with blue
		{
			char* name_with_extension = concatination(concatination(path,"/"), dir->d_name);
			struct stat statOffile;
			lstat(name_with_extension, &statOffile);
			count--;
			if (!count)
			{
				if (!isatty (1) || !colored)
					printf("%s%s%s\n",line, getLine(1), dir->d_name);
				else
					printf("%s%s%s%s%s\n",line, getLine(1), ftypeColor(&statOffile), dir->d_name, NORMAL_COLOR);
			}else
			{
				if (!isatty (1) || !colored)
					printf("%s%s%s\n",line, getLine(0), dir->d_name);
				else
				printf("%s%s%s%s%s\n",line, getLine(0), ftypeColor(&statOffile), dir->d_name, NORMAL_COLOR);
			}
		}

		else
			if(dir -> d_type == DT_DIR && strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 ) // if it is a directory
			{
				char* newline = concatination(line, getLine(2));
				if (!isatty (1) || !colored)
					printf("%s%s%s\n",line, getLine(0), dir->d_name);
				else
					printf("%s%s%s%s%s\n",line, getLine(0), GREEN, dir->d_name, NORMAL_COLOR); // print its name in green
				char d_path[255]; // here I am using sprintf which is safer than strcat
				sprintf(d_path, "%s/%s", path, dir->d_name);
				print_tree(d_path,newline); // recall with the new path & line
			}
	}
	closedir(d); // finally close the directory

}

char* ftypeColor(struct stat *sb)
{
	unsigned int i = (*sb).st_mode;
	switch (i & S_IFMT)
    {
		case S_IFBLK:
			return RED; //block device
		case S_IFCHR:
			return WHITE; //char device
		case S_IFDIR:
			return GREEN; //directory
		case S_IFIFO:
			return CYAN; //Fifo/pipe
		case S_IFLNK:
			return MAGENTA; //symlink
		case S_IFREG:
			return BLUE; //regular file
		case S_IFSOCK:
			return YELLOW; //socket
		default:
			return MAGENTA; //unknown
	}
}

size_t FilesCount(DIR * dir)
{
	size_t count = 0;
	struct dirent * d; // for the directory entries
	while ((d = readdir(dir)) != NULL)
    {
		if (d->d_type != DT_DIR)
			count++;
	}
	rewinddir(dir);
	return count;
}

char* getLine(unsigned int type)
{
	if (graphics)
	{
		switch (type)
		{
			case 0:
				return "\u251C\u2500\u2500\u2500\u2500\u2500";
			case 1:
				return "\u2514\u2500\u2500\u2500\u2500\u2500";
			case 2:
				return "\u2506\u2591\u2591\u2591\u2591\u2591";
			default:
				return "\u251C\u2500\u2500\u2500\u2500\u2500";
		}

	}
    else
	{
		switch (type)
		{
			case 0:
				return "|-----";
			case 1:
				return "`-----";
			case 2:
				return "|     ";
			default:
				return "|-----";
		}
	}
}