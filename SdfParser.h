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
 * - everything after # is considered a comment and ignored
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

//Types
typedef struct AstNode {
	struct AstNode* child;
	struct AstNode* sibling;
	char* value;
} AstNode;

//Function declarations
AstNode* parseFile(const char* filename);
void freeTree(AstNode* root);
void drawTree(FILE* dest, AstNode* root);

#ifdef	__cplusplus
}
#endif

#endif	/* SDFPARSER_H */

