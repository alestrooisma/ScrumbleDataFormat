/* 
 * File:   main.c
 * Author: Ale Strooisma
 *
 * Created on November 11, 2015, 6:01 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "SdfParser.h"

/*
 * Main function to test the SDF-parser
 */
int main(int argc, char** argv) {
	SdfNode* root = sdf_parse_file("test.sdf");
	if (root != NULL) {
		printf("parsing succesful!\n\n");
		sdf_draw_tree(stdout, root);
		sdf_free_tree(root);
	}
	return (EXIT_SUCCESS);
}

