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
	AstNode* root = parseFile("test.sdf");
	if (root != NULL) {
		printf("parsing succesful!\n\n");
		drawTree(stdout, root);
		freeTree(root);
	}
	return (EXIT_SUCCESS);
}

