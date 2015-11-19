#include "SdfParser.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
// Type declarations                                                          //
////////////////////////////////////////////////////////////////////////////////

#define INITIAL_BUFFER_SIZE 256

typedef enum {
	NO_ERROR, EXPECTED_END_OF_LINE, EXPECTED_LABEL, EXPECTED_ENTRY
} ErrorType;

typedef struct ParserData {
	FILE* file;
	const char* filename;
	size_t buffer_size;
	char* line;
	ssize_t linelength;
	int linenumber;
	ssize_t column;
	ErrorType error;
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

char* errormsg(ErrorType type) {
	switch (type) {
		case NO_ERROR:
			return "";
		case EXPECTED_END_OF_LINE:
			return "Expected end of line";
		case EXPECTED_LABEL:
			return "Expected label";
		case EXPECTED_ENTRY:
			return "Expected : or {";
	}
}

void* error(ParserData* data, ErrorType type) {
	data->error = type;

	printf("Error while parsing %s at line %i: %s.\n", data->filename, data->linenumber, errormsg(data->error));
	printf("%.*s", data->linelength, data->line);
	printf("%*s^\n\n", data->column, "");
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
	while (data->column < data->linelength && !isrestricted(data->line[data->column])) {
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
		if (data->column >= data->linelength || data->line[data->column] == '#') {
			next_line(data);
		}
		return true;
	}
	return false;
}

SdfNode* parse_entry(ParserData* data) {

	// First try to parse a label.
	char* label = 0;
	int length = consume_label(data, &label);
	if (length == 0) {
		// We did not find a label!
		return error(data, EXPECTED_LABEL);
	}

	// Create the node for this element
	SdfNode* element = new_valued_node(label, length);

	// We now need a { followed by a new-line.
	if (consume(data, '{')) {
		if (!next_line(data)) {
			// Expected new line!
			sdf_free_tree(element);
			return error(data, EXPECTED_END_OF_LINE);
		}

		// Parse children until the closing bracket is found.
		while (!consume(data, '}')) {
			SdfNode* child = parse_entry(data);
			if (child != NULL) {
				add_child(element, child);
			} else {
				// Syntax error in contents
				sdf_free_tree(element);
				return NULL;
			}
		}
	} else if (consume(data, ':')) {
		//TODO
	} else {
		sdf_free_tree(element);
		return error(data, EXPECTED_ENTRY);
	}

	// And we need a final newline, except when we are at the end of the file.
	if (!next_line(data) && !feof(data->file)) {
		// Encountered something else than a newline or feof
		sdf_free_tree(element);
		return error(data, EXPECTED_END_OF_LINE);
	}

	return element;
}

SdfNode* parse_document(ParserData* data) {
	SdfNode* root = new_valued_node(data->filename, strlen(data->filename));

	// Parse children
	while (!feof(data->file)) {
		SdfNode* child = parse_entry(data);
		if (child != NULL) {
			add_child(root, child);
		} else {
			// Syntax error in contents
			sdf_free_tree(root);
			return NULL;
		}
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
		printf("Unable to open Components file");
		return NULL;
	}

	// Prepare the parser status object
	ParserData data;
	data.file = file;
	data.filename = filename;
	data.buffer_size = INITIAL_BUFFER_SIZE * sizeof (char);
	data.line = malloc(data.buffer_size);
	data.linenumber = 0;
	data.error = NO_ERROR;
	next_line(&data);

	// Build the AST
	SdfNode* root = parse_document(&data);

	// Clean up before return
	fclose(file);
	free(data.line);

	// Return
	//	if (root == NULL) {
	//		return error("Parsing failed");
	//	}
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
