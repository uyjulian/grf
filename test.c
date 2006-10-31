#define GRF_NO_EXPORT
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <grf.h>

#ifndef __WIN32
struct timeval tv;

inline void timer_start() {
	gettimeofday((struct timeval *)&tv, NULL);
}

void timer_end(const char *reason) {
	struct timeval tv2;
	float diff;
	gettimeofday((struct timeval *)&tv2, NULL);
	diff = tv2.tv_usec - tv.tv_usec;
	diff = ((tv2.tv_sec - tv.tv_sec) * 1000) + (diff / 1000);
	printf(reason, diff);
}
#else
#define timer_start()
#define timer_end(x)
#endif

void test_grf_version() {
	uint32_t version=grf_version();
	uint8_t major, minor, revision;
	major = (version >> 16) & 0xff;
	minor = (version >> 8) & 0xff;
	revision = version & 0xff;
	printf(" - test_version(): %x (%d.%d.%d)\n", version, major, minor, revision);
	if ( (major != VERSION_MAJOR) || (minor != VERSION_MINOR) || (revision != VERSION_REVISION)) {
		puts("Error: This test program was not compiled for this lib!");
		exit(1);
	}
}

void test_new_handler() {
	void *handler;

	if (sizeof(struct grf_header) != GRF_HEADER_SIZE) {
		printf("Your compiler didn't use the right size for the packed structure (%x!=%x) !", (uint32_t) sizeof(struct grf_header), GRF_HEADER_SIZE);
		exit(2);
	}
	grf_set_compression_level(9);

	handler = grf_new("test.grf", true);
	printf(" - test_new_handler(): New handler at %p.\n", handler);
	if (grf_save(handler) == true) {
		puts(" - test_new_handler(): Write file success !");
	} else {
		perror("grf_save");
		grf_free(handler);
		exit(3);
	}
	grf_free(handler);
}

void test_load_file() {
	void *handler, *fhandler;
	void *filec;
	void **list;
//	char *fn = "/storage/win_d/Program Files/Gravity/fRO_II/data.grf";
//	char *fn = "/storage/win_d/Program Files/Gravity/20060224_krodata.gpf";
	char *fn = "103-6.grf";
	char *fn2 = "DATA/texTURE\\유저인터페이스/LOADING45.JPG";

// test
#if 0
handler=grf_load("103.grf", true);
grf_save(handler);
grf_free(handler);
#endif
	printf(" - test_load_file(): Opening `%s` in read only mode...\n", fn);
	timer_start();
	handler = grf_load(fn, false);
	printf(" - test_load_file(): Loaded file at %p.\n", handler);
	timer_end(" - test_load_file(): File loading took %fms\n");
	if (handler == NULL) return;
	printf(" - test_load_file(): There are %d files in this GRF.\n", grf_filecount(handler));
	printf(" - test_load_file(): %d byte(s) wasted.\n", grf_wasted_space(handler));
	printf(" - grf_create_tree(): Building global tree...\n");
	timer_start();
	grf_create_tree(handler);
	timer_end(" - grf_create_tree(): Tree created in %fms\n");
	printf(" - test_load_file(): Searching for file %s\n", fn2);
	timer_start();
	fhandler = grf_get_file(handler, fn2);
	timer_end(" - test_load_file(): File search took %fms.\n");
	printf(" - test_load_file(): File is at %p\n", fhandler);
	if (fhandler != NULL) {
		printf(" - test_load_file(): Real filename is `%s`\n", grf_file_get_filename(fhandler));
		printf(" - test_load_file(): File size is %d bytes.\n", grf_file_get_size(fhandler));
		filec = malloc(grf_file_get_size(fhandler));
		printf(" - test_load_file(): Extracted %d bytes for this file.\n", grf_file_get_contents(fhandler, filec));
#if 0
		FILE *f=fopen("loading45.jpg", "w");
		if (f == NULL) {
			printf(" - test_writing failed\n");
		} else {
			printf(" - test_writing ok\n");
			fwrite(filec, grf_file_get_size(fhandler), 1, f);
			fclose(f);
		}
#endif
		free(filec);
	}
	if (grf_filecount(handler)<=16) {
		// Attempt to list file content
		printf(" - test_load_file(): Getting files list\n");
		timer_start();
		list = grf_get_file_list(handler);
		timer_end(" - test_load_file(): Got files list in %fms.\n");
		if (list == NULL) {
			printf(" - test_load_file(): No files found\n");
		} else {
			for(int i=0;list[i]!=NULL;i++) {
				printf(" - test_load_file(): %s\n", grf_file_get_filename(list[i]));
			}
			free(list);
		}
	}
	grf_free(handler);
}

int main() {
	puts(grf_versionstring());
	puts(" * Running library tests...");
	test_grf_version();
	test_new_handler();
	test_load_file();
	return 0;
}

