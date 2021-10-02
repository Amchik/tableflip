#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include "include/tableflip.h"

int main(int argc, char** argv) {
	tftable_t table;
	size_t len;
	char* file_contents;
	FILE* fp;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return(1);
	}

	fp = fopen(argv[1], "r");
	if (errno != 0) {
		perror("fopen()");
		return(2);
	}
	if (fp == 0) {
		fprintf(stderr, "fopen(): Something went wrong\n");
		return(2);
	}
	
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	file_contents = malloc(len + 1);
	file_contents[len] = '\0';
	fread(file_contents, sizeof(char), len, fp);
	fclose(fp);

	table = tfparse(file_contents);

	if (table.node == 0) {
		fprintf(stderr, "Error: empty file passed\n");
		return(3);
	}	else if (table.code != tfe_none) {
		tferror(table, file_contents);

		return(3 + table.code);
	}

	tfprint(table.node);

	return(0);
}
