#include "SdfParser.h"

#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Type declarations                                                          //
////////////////////////////////////////////////////////////////////////////////

typedef struct Document {
	FILE* file;
	const char* filename;
} Document;

////////////////////////////////////////////////////////////////////////////////
// Helper functions                                                           //
////////////////////////////////////////////////////////////////////////////////

SdfNode* new_node() {
	SdfNode* node = malloc(sizeof(SdfNode));
	node->child=NULL;
	node->sibling=NULL;
	node->value=NULL;
	return node;
}

void* error(const char* msg) {
	printf(msg);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Parsing functions                                                          //
////////////////////////////////////////////////////////////////////////////////

SdfNode* parse_document(Document* this) {
	SdfNode* node = new_node();
	node->value = malloc(sizeof(char)*(strlen(this->filename)+1));
	strcpy(node->value, this->filename);
	return node;
}

////////////////////////////////////////////////////////////////////////////////
// Output functions                                                           //
////////////////////////////////////////////////////////////////////////////////

void draw_subtree(FILE* dest, SdfNode* node, int indent) {
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
		draw_subtree(dest, node->child, indent+1);
	}
	if (node->sibling != NULL) {
		draw_subtree(dest, node->sibling, indent);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Public functions                                                           //
////////////////////////////////////////////////////////////////////////////////

SdfNode* sdf_parse_file(const char* filename) {
	// Open the file
	FILE * file = fopen(filename, "r");

	// Check to see if we actually opened the file OK
	if (file == NULL) {
		return error("Unable to open Components file");
	}
	
	// Prepare the parser status object
	Document obj;
	obj.filename = filename;
	
	// Build the AST
	SdfNode* root = parse_document(&obj);
	if (root == NULL) {
		return error("Parsing failed");
	}

	return root;
}

void sdf_free_tree(SdfNode* root) {
	if(root->child != NULL) {
		sdf_free_tree(root->child);
	}
	if(root->sibling != NULL) {
		sdf_free_tree(root->sibling);
	}
	free(root->value);
	free(root);
}

void sdf_draw_tree(FILE* dest, SdfNode* root) {
	draw_subtree(dest, root, 0);
}
