/* 
 * This file provides a parser for the ScrumbleDataFormat (SDF).
 * 
 * --- CONTEXT FREE GRAMMAR ---
 * 
 * document      -> element* EOF
 * element       -> LABEL LBRACE NEWLINE item* RBRACE NEWLINE
 * item          -> LABEL ( singleValue | structValue ) NEWLINE
 * singleValue   -> COLON ( quotedString | LABEL )
 * structValue   -> LBRACE NEWLINE item* RBRACE
 * quotedString  -> QUOTE STRING QUOTE
 * 
 * LABEL         -> any characters except: restricted characters, leading and trailing whitespace
 * STRING        -> any characters except NEWLINE - note that you can escape characters with \
 * 
 * Restricted characters: NEWLINE, LBRACE, RBRACE, COLON, QUOTE, NUMBERSIGN
 * 
 * --- ADDITIONAL SYNTAX NOTES ---
 * 
 * - everything after a # is considered a comment and ignored
 * - empty lines as well as leading and trailing whitespace is ignored
 * - whitespace between tokens is ignored as well
 * 
 * Author: Ale Strooisma
 * Created on November 11, 2015, 6:02 PM
 */

#ifndef SDFPARSER_H
#define	SDFPARSER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * The node type that makes up the abstract syntax trees (AST) produced by the 
 * SDF-parser.
 */
typedef struct SdfNode {
	struct SdfNode* child;
	struct SdfNode* sibling;
	char* value;
} SdfNode;

/**
 * Parses the file with the given filename as SDF and returns the abstract 
 * syntax tree (AST). Returns NULL if errors are encountered.
 * The AST needs to be freed by calling sdf_free_tree with the node returned by 
 * this function as argument.
 * 
 * @param filename the name of the file to parse
 * @return the AST of the given file or NULL if errors are encountered
 */
SdfNode* sdf_parse_file(const char* filename);

/**
 * Frees the (sub)tree of which the given node is the root. Do not pass NULL to
 * this function.
 * 
 * @param root the root node of the tree that must be freed
 */
void sdf_free_tree(SdfNode* root);

/**
 * Pretty prints the (sub)tree of which the given node is the root to the given 
 * file handle. Note that stdout is a valid value for dest.
 * 
 * @param dest the file or stream to which the AST must be printed
 * @param root the root node of the tree that must be printed.
 */
void sdf_draw_tree(FILE* dest, SdfNode* root);

#ifdef	__cplusplus
}
#endif

#endif	/* SDFPARSER_H */

