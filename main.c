#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE
#include <libexif/exif-data.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
//linux only
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

mode_t fileInfo(char *path);

int timeffile(struct tm *tm, char *file_name) { // TODO(robin): https://sourceforge.net/p/libexif/mailman/message/2898934/ 
	int ret = 0;
	ExifData *exif;
	ExifEntry *entry;

	exif = exif_data_new_from_file(file_name);
	if (exif != NULL) {
		entry = exif_content_get_entry(exif->ifd[EXIF_IFD_EXIF], EXIF_TAG_DATE_TIME_ORIGINAL);
		if (strptime((const char *)entry->data, "%Y:%m:%d %H:%M:%S", tm) != NULL) {
			ret = 1; // la convertion de char* vers tm a réusie
		} else {
			fprintf(stderr, "Erreur format date exif: %s\n", entry->data);
		}
		exif_data_unref(exif);
	} else {
		fprintf(stderr, "Erreur pas de données exif dans %s\n", file_name);
	}
	return ret;
}

void disptime(struct tm *tm) {
	char buf[20];
        (void) strftime(buf, sizeof(buf), "%d %b %Y %H:%M", tm);
	(void) puts(buf);
}

char* insereDate(char *file_name, struct tm *tm) {
	char *pos_ptr;
	char *buff;	
	char date[11];
	pos_ptr = strrchr(file_name, '/');

	if (strchr(pos_ptr, '-') != NULL) { // la photo a probablement déja été renomer
		return NULL;
	}

	(void) strftime(date, 11, "%F", tm);
	buff = malloc((strlen(file_name) + sizeof(date) + 2) * sizeof(char)); //P2011-09-02 1200859.JPG

	if (pos_ptr == NULL) {
        	(void) sprintf(buff, "P%s %s", date, file_name + sizeof(char));
	} else {
		char *trunc;
		int pos;
	       	pos = pos_ptr - file_name; // calcul sur pointeur
		trunc = malloc((pos + 2) * sizeof(char)); // 2 : pos commance par 0 et terminer la chaine
		(void) strncpy(trunc, file_name, pos + 2); // pos + 2 car il faut terminer la chaine sans supprimer d'info
		trunc[pos + 1] = '\0'; // terminer la chaine
		(void) sprintf(buff, "%sP%s %s", trunc, date, pos_ptr + 2 * sizeof(char));
	        free(trunc);	
	}
	return buff;
}

void processFile(char *file_name) {
	struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
	printf("Traitement du fichier %s\n", file_name);
	if (timeffile(&tm, file_name)) {
		char *date = insereDate(file_name, &tm);
		if (date != NULL) {
			rename(file_name, date);
		} else {
			printf("Le fichier a déjà été renomer.\n");
		}
		free(date);
	}
}

void processFolder(char *folder_name, bool recursive) {
	DIR *d;
	struct dirent *dir;
	d = opendir(folder_name);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
				char *fn;
				fn = (char *) malloc(strlen(folder_name) + strlen(dir->d_name) + 2);
				*fn = '\0';				

				strcat(fn, folder_name);
				strcat(fn, "/");
				strcat(fn, dir->d_name);

				if (recursive && dir->d_type == DT_DIR) {
					processFolder(fn, recursive);
				} else if (dir->d_type == DT_REG) {
					processFile(fn);
				} else if (dir->d_type == DT_UNKNOWN) {
					mode_t fi = fileInfo(fn);
					if (recursive && S_ISDIR(fi)) {
						processFolder(fn, recursive);
					} else if (S_ISREG(fi)) {
						processFile(fn);
					}
				}
				free(fn);
			}
		}
		//printf("fermeture de %s\n", folder_name);
		closedir(d);
	}
}

mode_t fileInfo(char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) == -1) {
		fprintf(stderr, "Error fileInfo(%s): %s\n", path, strerror(errno));
	}
	return statbuf.st_mode;
}

int main(int argc, char** argv) {
	bool recursive = false;
	bool arg_parse_error = false;
	int index;
	int opt;
	while ((opt = getopt(argc, argv, "r")) != -1) {
		switch (opt) {
		case 'r': recursive = true; break;
		default:
			 arg_parse_error = true;
		}
	}
	if (argc == optind || arg_parse_error) {
		fprintf(stderr, "Usage: %s [-r] [file|directory]...\n", argv[0]);
		return EXIT_FAILURE;
	}
	for (index = optind; index < argc; index++) {
		char* path = argv[index];
		mode_t fi = fileInfo(path);
		if (S_ISDIR(fi)) {
			processFolder(path, recursive);
		} else if (S_ISREG(fi)) {
			processFile(path);
		}
	}
	return EXIT_SUCCESS;
}
