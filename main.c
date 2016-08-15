#define _XOPEN_SOURCE
#include <libexif/exif-data.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void timeffile(struct tm *tm, char *file_name) {
	ExifData *exif;
	ExifEntry *entry;

	exif = exif_data_new_from_file(file_name);
	entry = exif_content_get_entry(exif->ifd[EXIF_IFD_EXIF], EXIF_TAG_DATE_TIME_ORIGINAL);

	strptime((const char *)entry->data, "%Y:%m:%d %H:%M:%S", tm);
	
//	exif_entry_unref(entry); // libere la mémoire (unref appele free)
	exif_data_unref(exif);
}

void disptime(struct tm *tm) {
	char buf[20];
        strftime(buf, sizeof(buf), "%d %b %Y %H:%M", tm);
	puts(buf);
}

int main() {
	struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
	timeffile(&tm, "img/P1250942.JPG");
	disptime(&tm);
	return EXIT_SUCCESS;
}
