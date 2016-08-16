#define _XOPEN_SOURCE
#include <libexif/exif-data.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
//linux only 
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>

int isDirectory(char *path);

int timeffile(struct tm *tm, char *file_name) {
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
	} else {
		fprintf(stderr, "Erreur pas de données exif dans %s\n", file_name);
	}

	exif_data_unref(exif);
	return ret;
}

void disptime(struct tm *tm) {
	char buf[20];
        (void) strftime(buf, sizeof(buf), "%d %b %Y %H:%M", tm);
	(void) puts(buf);
}

void processFile(char *file_name) {
	struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
	printf("\tTraitement du fichier %s\n", file_name);
	if (timeffile(&tm, file_name)) {
		disptime(&tm);
	}
}

void processFolder(char *folder_name) {
	DIR *d;
	struct dirent *dir;
	d = opendir(folder_name);
	//printf("\nouverture de %s\n", folder_name);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
//			printf("%s\n", dir->d_name);
			if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
				char *fn;
				fn = (char *) malloc(strlen(folder_name) + strlen(dir->d_name) + 2);
				*fn = '\0';				

				strcat(fn, folder_name);
				strcat(fn, "/");
				strcat(fn, dir->d_name);

				if (isDirectory(fn)) {
					//printf("\n%s est un répertoire\n", fn);
					processFolder(fn);
				} else {
					//printf("%s est un fichier\n", fn);
					processFile(fn);
				}
				free(fn);
			}
		}
		//printf("fermeture de %s\n", folder_name);
		closedir(d);
	}
}

int isDirectory(char *path) {
	int res = -1;
	struct stat statbuf;
	if (stat(path, &statbuf) == -1) {
		fprintf(stderr, "Error isDirectory(%s): %s\n", path, strerror(errno));
	} else {
		res = S_ISDIR(statbuf.st_mode);
	}
	return res;
}

int main() {
	/*struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
	timeffile(&tm, "img/P1250942.JPG");
	disptime(&tm);*/
	processFolder("img");
	return EXIT_SUCCESS;
}
