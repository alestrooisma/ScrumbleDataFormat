#include "SdfParser.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
// Type declarations                                                          //
////////////////////////////////////////////////////////////////////////////////

#define INITIAL_BUFFER_SIZE 256

typedef struct ParserData {
	FILE* file;
	const char* filename;
	size_t buffer_size;
	char* line;
	ssize_t linelength;
	int linenumber;
	ssize_t column;
} ParserData;

////////////////////////////////////////////////////////////////////////////////
// Helper functions                                                           //
////////////////////////////////////////////////////////////////////////////////

void copy_value(SdfNode* node, const char* string, int length) {
	node->value = malloc(sizeof (char)*(length + 1));
	strncpy(node->value, string, length);
	node->value[length] = '\0';
	node->value_length = length;
}

SdfNode* new_node() {
	SdfNode* node = malloc(sizeof (SdfNode));
	node->child = NULL;
	node->sibling = NULL;
	node->value = NULL;
	return node;
}

SdfNode* new_valued_node(const char* string, int length) {
	SdfNode* node = new_node();
	copy_value(node, string, length);
	return node;
}

/**
 * Adds a child to the parent, by adding it as a sibling of its last child.
 */
void add_child(SdfNode* parent, SdfNode* newchild) {
	if (parent->child == NULL) {
		parent->child = newchild;
	} else {
		SdfNode* sibling = parent->child;
		while (sibling->sibling != NULL) {
			sibling = sibling->sibling;
		}
		sibling->sibling = newchild;
	}
}

void* error(const char* msg) {
	printf("%s\n", msg);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Parsing functions                                                          //
////////////////////////////////////////////////////////////////////////////////

bool isrestricted(char ch) {
	return ch == '{' || ch == '}' || ch == ':' || ch == '"' || ch == '#';
}

void consume_whitespace(ParserData* data) {
	while (isspace(data->line[data->column])) {
		data->column++;
	}
}

bool consume(ParserData* data, char ch) {
	bool found = data->line[data->column] == ch;
	if (found) {
		data->column++;
	}
	return found;
}

int consume_label(ParserData* data, char** label) {
	int start = data->column;
	*label = &(data->line[start]);
	while (!isrestricted(data->line[data->column])) {
		data->column++;
	}
	int length = data->column - start;
	while (length > 0 && isspace(data->line[start + length - 1])) {
		length--;
	}
	return length;
}

bool next_line(ParserData* data) {
	//TODO error/warning on remaining text!
	if (!feof(data->file)) {
		data->linelength = getline(&(data->line), &(data->buffer_size), data->file);
		data->linenumber++;
		data->column = 0;
		consume_whitespace(data);
		if (data->line[data->column] == '#' || data->column >= data->linelength) {
			next_line(data);
		}
		return true;
	}
	return false;
}

SdfNode* parse_item(ParserData* data) {
	next_line(data);
	return NULL;
}

void parse_children(ParserData* data, SdfNode* parent);

SdfNode* parse_entry(ParserData* data) {
	
	// First try to parse a label.
	char* label = 0;
	int length = consume_label(data, &label);
	if (length == 0) {
		// We did not find a label!
		// TODO set error
		return NULL;
	}
	
	// Create the node for this element
	SdfNode* element = new_valued_node(label, length);
	
	// We now need a { followed by a new-line.
	if (!(consume(data, '{') && next_line(data))) {
		// TODO set error
		sdf_free_tree(element);
		return NULL;
	}
	
	// Parse the contents of the element, until we find the closing bracket.
	while (consume(data, '}') == false) {
		next_line(data);
//		parse_entry(data);
	}

	// And we need a final newline, except when we are at the end of the file.
	if (!next_line(data) && !feof(data->file)) {
		// Encountered something else than a newline or feof
		// TODO set error
		sdf_free_tree(element);
		return NULL;
	}
	
	return element;
}

void parse_children(ParserData* data, SdfNode* parent) {
	SdfNode* child;
	while ((child = parse_entry(data)) != NULL) {
		add_child(parent, child);
	}
}

SdfNode* parse_document(ParserData* data) {
	SdfNode* root = new_valued_node(data->filename, strlen(data->filename));
	parse_children(data, root);
	
	// If end of file has been reached, parsing was successful.
	// If not at end-of-file, this means the last call to parse_element 
	// returned NULL because it encountered a syntax error!
	if (feof(data->file)) {
		// Success: return the tree.
		return root;
	} else {
		// Syntax error! Display it, clean up, and return null.
		
		// TODO - display error
		
		sdf_free_tree(root);
		return NULL;
	}

	return root;
}

////////////////////////////////////////////////////////////////////////////////
// Output functions                                                           //
////////////////////////////////////////////////////////////////////////////////

void draw_subtree(FILE* dest, SdfNode* node, int indent) {
	int i;
	for (i = 0; i < indent; i++) {
		fprintf(dest, "  ");
	}
	if (node->value != NULL) {
		fprintf(dest, "%s\n", node->value);
	} else {
		fprintf(dest, "NULL\n");
	}
	if (node->child != NULL) {
		draw_subtree(dest, node->child, indent + 1);
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
	ParserData data;
	data.file = file;
	data.filename = filename;
	data.buffer_size = INITIAL_BUFFER_SIZE * sizeof (char);
	data.line = malloc(data.buffer_size);
	data.linenumber = 0;
	next_line(&data);

	// Build the AST
	SdfNode* root = parse_document(&data);

	// Clean up before return
	fclose(file);
	free(data.line);

	// Return
	if (root == NULL) {
		return error("Parsing failed");
	}
	return root;
}

void sdf_free_tree(SdfNode* root) {
	if (root->child != NULL) {
		sdf_free_tree(root->child);
	}
	if (root->sibling != NULL) {
		sdf_free_tree(root->sibling);
	}
	free(root->value);
	free(root);
}

void sdf_draw_tree(FILE* dest, SdfNode* root) {
	draw_subtree(dest, root, 0);
}
