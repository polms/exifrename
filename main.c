#define _XOPEN_SOURCE
#include <libexif/exif-data.h>
#include <libexif/exif-log.h>
#include <libexif/exif-mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	ExifData *exif;
	ExifEntry *entry;
	char date[20];
	char buf[150];
	struct tm tm;

	exif = exif_data_new_from_file("img/P1250942.JPG");
	entry = exif_content_get_entry(exif->ifd[EXIF_IFD_EXIF], EXIF_TAG_DATE_TIME_ORIGINAL);
	exif_entry_get_value(entry, date, 20);
	printf("%s\n", date);

	strptime(date, "%Y:%m:%d %H:%M:%S", &tm);
        strftime(buf, sizeof(buf), "%d %b %Y %H:%M", &tm);
	puts(buf);
	return EXIT_SUCCESS;
}
