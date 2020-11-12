#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

static void pf(size_t lvl, const char *t, const char *name)
{
	while (lvl--)
		printf(" ");
	printf("%s: %s\n", t, name);
}

int sss(size_t lvl, const char *dname)
{
	struct stat st;
	struct dirent *ent;
	DIR *d;
	int c = 0;
	char n[PATH_MAX + 1];
	d = opendir(dname);
	while (d && (ent = readdir(d)))
	{
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
			continue;
		sprintf(n, "%s/%s", dname, ent->d_name);
		stat(n, &st);
		if (S_ISDIR(st.st_mode))
		{
			c += sss(lvl + 1, n);
		}
		else
		{
			pf(lvl + 1, "file", n);
			c += st.st_size;
		}
	}
	if (d)
		closedir(d);
	return c;
}

void pr(int tgid)
{
	char path[40], line[100], *p;
	FILE *statusf;

	snprintf(path, 40, "/proc/%d/status", tgid);

	statusf = fopen(path, "r");
	if (!statusf)
		return;

	while (fgets(line, 100, statusf))
	{
		if (strncmp(line, "State:", 6) != 0)
			continue;
		// Ignore "State:" and whitespace
		p = line + 7;
		while (isspace(*p))
			++p;

		printf("%6d %s", tgid, p);
		break;
	}
	fclose(statusf);
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("?./try\n./l2 -h\n./l2 --help\n");
		exit(1);
	}

	FILE *fpr, *fpw;

	char ch;

	DIR *opendir(const char *name);
	struct dirent *pdir = NULL;
	DIR *pd = NULL;
	struct stat st;

	int c = 0;

	char *opt = argv[1];

	if (opt[1] == '-')
		opt[1] = 'h';
	if (strcmp(opt, "-hhelp") && (opt[2] != NULL))
	{
		printf("?\ntry\n./l2 -h\n./l2 --help\n");
		exit(1);
	}
	switch (opt[1])
	{
	default:
		printf("?\ntry\n./l2 -h\n./l2 --help\n");
		break;
	case 'h':
		printf("Авторы: Солдатов Даниил\n\tКустов Даниил\n\tКлепцов Дмитрий\nВсе доступные аргументы:\n\t-m - Перемещение файла\n\t-c - Копирование файла\n\t-d - удаление файла\n\t-s - Подсчет общего размера указанной директории\n\t-l - Отображение всех файлов в указанной директории\n\t-p - Отображение всех процессов из файловой системы procfs");
		printf("\nКраткое описание проекта:\n\tПроект позволяет управлять файловой системой\n");
		printf("Примеры запуска:\n\t./l2 -m <путь к файлу> <пункт назначения файла\n");
		printf("\t./l2 -c <путь к файлу> <пункт назначениф файла>\n\t./l2 -d <путь к файлу>");
		printf("\n\t./l2 -s <директория>\n\t./l2 -l <директория>\n");
		printf("\t<Путь к файлу>/<пункт назначения файла> для -m, -c, -d может быть путем к файлу </home/admin/text.txt> или именем файла в текущей папке <text.txt>\n");
		printf("\t<Директория> для -s может быть путем </home/admin/>, текущей директорией <.>, файлом в текущей дир. <text.txt>, файлом в другой дир. </home/admin/text.txt>\n");
		break;
	case 'l':
		if (argc < 3)
		{
			printf("./l2 -l <директория>\n");
			break;
		}
		pd = opendir(argv[2]);
		while ((pdir = (readdir(pd))) != NULL)
		{
			printf("%s\n", pdir->d_name);
		}
		closedir(pd);
		break;
	case 's':
		if (argc < 3)
		{
			printf("./l2 -s <директория>\n");
			break;
		}
		stat(argv[2], &st);
		if (S_ISDIR(st.st_mode))
		{
			printf("%d\n", sss(0, argv[2]));
		}
		else
		{
			printf("%ld\n", st.st_size);
		}
		break;
	case 'm':
		if (argc < 3)
			printf("./l2 -m <путь к файлу> <пункт назначения файла>\n");
		rename(argv[2], argv[3]);
		break;
	case 'c':
		if (argc < 3)
			printf("./l2 -c <путь к файлу> <пункт назначения файла>\n");
		if ((fpr = fopen(argv[2], "rb")) == NULL)
		{
			printf("Error\n");
		}
		if ((fpw = fopen(argv[3], "rb")) != NULL)
		{
			printf("File exist\n");
			exit(1);
		}
		if ((fpw = fopen(argv[3], "wb")) == NULL)
		{
			printf("Error 2\n");
		}
		while ((ch = getc(fpr)) != EOF)
			putc(ch, fpw);
		fclose(fpw);
		fclose(fpr);
		break;
	case 'd':
		if (argc < 2)
			printf("./l2 -d <путь к файлу>\n");
		if (remove(argv[2]))
			printf("Error\n");
		break;
	case 'p':
		pd = opendir("/proc");
		if (pd == NULL)
			printf("Error open proc\n");
		while (pdir = readdir(pd))
		{
			if (!isdigit(*pdir->d_name))
				continue;
			c = strtol(pdir->d_name, NULL, 10);
			pr(c);
		}
		break;
	}
}