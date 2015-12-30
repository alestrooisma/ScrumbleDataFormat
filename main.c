/* 
 * File:   main.c
 * Author: Ale Strooisma
 *
 * Created on November 11, 2015, 6:01 PM
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>

#include "SdfParser.h"

#define MAX 100

void processDirectory(char* path, char* prefix) {
	char* list[MAX];
	int i;
	for (i = 0; i < MAX; i++) {
		list[i] = NULL;
	}

	int prefix_length = strlen(prefix);
	DIR *dir;
	struct dirent *entry;
	if ((dir = opendir(path)) != NULL) {
		while ((entry = readdir(dir)) != NULL) {
			char* fname = entry->d_name;
			int fname_length = strlen(fname);
			if (strncmp(fname, prefix, prefix_length) == 0) {
				int number = atoi(&fname[prefix_length]);
				if (number < MAX) {
					list[number] = (char*) malloc(sizeof (char) * (strlen(path) + fname_length + 1));
					strcpy(list[number], path);
					strcat(list[number], fname);
				}
			}
		}
		closedir(dir);
	} else {
		/* could not open directory */
		perror("");
	}

	for (i = 0; i < MAX; i++) {
		if (list[i]) {
			printf("Parsing %s\n", list[i]);
			SdfNode* root = sdf_parse_file(list[i]);
			if (root) {
				sdf_draw_tree(stdout, root);
				sdf_free_tree(root);
				printf("Success!\n\n");
			}
			free(list[i]);
		}
	}
}

/**
 * Main function to test the SDF-parser
 */
int main(int argc, char** argv) {
	processDirectory("sampledata/correct/", "correct");
	//	SdfNode* root = sdf_parse_file("sampledata/asteroidtypes.sdf");
	//	if (root != NULL) {
	//		printf("parsing successful!\n\n");
	//		sdf_draw_tree(stdout, root);
	//		sdf_free_tree(root);
	//		return (EXIT_SUCCESS);
	//	} else {
	//		printf("parsing failed...\n");
	//		return (EXIT_FAILURE);
	//	}
}