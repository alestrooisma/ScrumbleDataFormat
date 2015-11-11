#include "SdfParser.h"

#include <stdlib.h>

// Helpers

AstNode* newNode() {
	AstNode* node = malloc(sizeof(AstNode));
	node->child=NULL;
	node->sibling=NULL;
	node->value=NULL;
	return node;
}

void* error(const char* msg) {
	printf(msg);
	return NULL;
}

// Declarations
AstNode* parseDocument();

// Parser implementations

AstNode* parseFile(const char* filename) {
	//Open the file with material definitions
	FILE * file = fopen(filename, "r");

	//Check to see if we actually opened the file OK
	if (file == NULL) {
		return error("Unable to open Components file");
	}
	
	//Build the AST
	AstNode* root = parseDocument(file);
	if (root == NULL) {
		return error("Parsing failed");
	}

	return root;
}

void freeTree(AstNode* root) {
	if(root->child != NULL) {
		freeTree(root->child);
	}
	if(root->sibling != NULL) {
		freeTree(root->sibling);
	}
	free(root->value);
	free(root);
}

AstNode* parseDocument() {
	AstNode* node = newNode();
	return node;
}

// AST Drawing functions

void drawSubTree(FILE* dest, AstNode* node, int indent) {
	int i;
	for(i = 0; i < indent; i++) {
		fprintf(dest, "  ");
	}
	if (node->value != NULL) {
		fprintf(dest, "%s\n", node->value);
	} else {
		fprintf(dest, "NULL\n");
	}
	if (node->child != NULL) {
		drawSubTree(dest, node->child, indent+1);
	}
	if (node->sibling != NULL) {
		drawSubTree(dest, node->sibling, indent);
	}
}

void drawTree(FILE* dest, AstNode* root) {
	drawSubTree(dest, root, 0);
}