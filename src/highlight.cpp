/*
    FLDev - FLTK Integrated Development Environment
    Copyright (C) 2005-2016  Philipp Pracht, Georg Potthast, Neil McNeight

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "extras.h" 
#include "Fl_Dev_Code_Browser.h"
#include "highlight.h"
#include "globals.h"


Fl_Text_Buffer     *textbuf = 0;
Fl_Text_Buffer     *stylebuf = 0;
Fl_Text_Buffer		*op_stylebuf = 0;
Fl_Text_Editor_ext 		*te;



//Editor colors
Fl_Color hl_plain = FL_BLACK;
Fl_Color hl_linecomment = FL_DARK_GREEN;
Fl_Color hl_blockcomment = FL_DARK_GREEN;
Fl_Color hl_string = FL_BLUE;
Fl_Color hl_directive = FL_DARK_MAGENTA;
Fl_Color hl_type = FL_DARK_RED;
Fl_Color hl_keyword = FL_BLUE;
Fl_Color hl_character = FL_DARK_RED;
Fl_Color background_color = FL_WHITE;


const char         *code_keywords[] = {	// List of known C/C++ keywords...
		     "and",
		     "and_eq",
		     "asm",
		     "bitand",
		     "bitor",
		     "break",
		     "case",
		     "catch",
		     "compl",
		     "continue",
		     "default",
		     "delete",
		     "do",
		     "else",
		     "false",
		     "for",
		     "goto",
		     "if",
		     "new",
		     "not",
		     "not_eq",
		     "operator",
		     "or",
		     "or_eq",
		     "return",
		     "switch",
		     "template",
		     "this",
		     "throw",
		     "true",
		     "try",
		     "using",
		     "while",
		     "xor",
		     "xor_eq"
		   };
const char         *code_types[] = {	// List of known C/C++ types...
		     "auto",
		     "bool",
		     "char",
		     "class",
		     "const",
		     "const_cast",
		     "double",
		     "dynamic_cast",
		     "enum",
		     "explicit",
		     "extern",
		     "float",
		     "friend",
		     "inline",
		     "int",
		     "long",
		     "mutable",
		     "namespace",
		     "private",
		     "protected",
		     "public",
		     "register",
		     "short",
		     "signed",
		     "sizeof",
		     "static",
		     "static_cast",
		     "struct",
		     "template",
		     "typedef",
		     "typename",
		     "union",
		     "unsigned",
		     "virtual",
		     "void",
		     "volatile"
		   };
//
// 'style_parse()' - Parse text and produce style data.
//
//
void
style_parse(const char *text,
            char       *style,
	    int        length) {
  char	     current; 
  int	     last;
  char	     buf[255],
             *bufptr;
  const char *temp; 

  if(!cppfile) return;

  for (current = *style, last = 0; length > 0; length --, text ++) {
    
    //if ((current != 'C') && (current != 'D') && (current != 'E')) current = 'A';
    //if ((current == 'B')) current = 'A';
  
    if (current == 'A') {
      // Check for directives, comments, strings, and keywords...
      	if (*text == '#') {
        	// Set style to directive
	        current = 'E';
		} 
	
		else if (strncmp(text, "//", 2) == 0) {
	        current = 'B';
			for (; length > 0 && *text != '\n'; length --, text ++) 
				*style++ = 'B';

	        if (length == 0) break;
      	} 

		else if (strncmp(text, "/*", 2) == 0) {
	        	current = 'C';
			*style++ = current;
			*style--;
		} 

		else if (strncmp(text, "\\\"", 2) == 0) {
		        // Quoted quote...
			*style++ = current;
			*style++ = current;
			text ++;
			length --;
			continue;
      	} 

		else if (*text == '\"' /*| *text == '\''*/) {
        	current = 'D';
      	} 

		else if (*text == '\'') {
        	current = 'H';
      	} 

		else if (!last && islower(*text) && text > 0 && !(isalnum(*(text-1)) || *(text-1)=='_')) {
        	// Might be a keyword...
			for (temp = text, bufptr = buf; (isalnum(*temp) || *temp=='_') && bufptr < (buf + sizeof(buf) - 1); *bufptr++ = *temp++);
		        if (!islower(*temp)) {
					*bufptr = '\0';
					bufptr = buf;
	
					if (bsearch(&bufptr, code_types, sizeof(code_types) / sizeof(code_types[0]), sizeof(code_types[0]), compare_keywords)) {
						while (text < temp) {
					  		*style++ = 'F';
							text ++;
			      			length --;
						}
	
	    				text --;
	    				length ++;
	    				last = 1;
	    				continue;
	    				
		  			} else if (bsearch(&bufptr, code_keywords, sizeof(code_keywords) / sizeof(code_keywords[0]), sizeof(code_keywords[0]), compare_keywords)) {
		    				while (text < temp) {
		      					*style++ = 'G';
		      					text ++;
		      					length --;
		    				}
	
		    				text --;
		    				length ++;
		    				last = 1;
		    				continue;
		  			}
				}
     	}
 	} else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
   		// Close a C comment... 

    		*style++ = current;
   		*style++ = current;
    		text ++;
   		length --;
    		current = 'A';
   		continue;
 	} else if (current == 'D') {
   		// Continuing in string...
   		if (strncmp(text, "\\\"", 2) == 0 || strncmp(text, "\\\'", 2) == 0) {
      		// Quoted end quote...
			*style++ = current; 
			*style++ = current;
			text ++;
			length --;
			continue;
   		} else if (*text == '\"'/* || *text == '\''*/) {
       		// End quote...
			*style++ = current;
			current = 'A';
			continue;
   		}
 	} else if (current == 'H') {
   		// Continuing in char...
   		if (strncmp(text, "\\\'", 2) == 0) {
      		// Quoted end quote...
			*style++ = current; 
			*style++ = current;
			text ++;
			length --;
			continue;
   		} else if (*text == '\'') {
     		// End quote...
			*style++ = current;
			current = 'A';
			continue;
   		}
   	}


   	// Copy style info...
   	if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
   	/*else if(current == 'E' && strncmp(text, "/*", 2) == 0) {
   		*style++ = 'C';
   		*style++ = 'C';
   		//*style--;
   	}*/
   	else if(current == 'E' && strncmp(text, "/*", 2) == 0)
   	{
   		*style++ = 'C';      			
		current = 'C';
   	}
   	else if(current == 'E' && strncmp(text, "//", 2) == 0)
   	{
   		*style++ = 'B';      			
		current = 'B';
   	}
   	else *style++ = current;

   	last = isalnum(*text) || *text == '.';

   	if (*text == '\n') {
     	// Reset column and possibly reset the style
     		if (current != 'D' && current != 'C') current = 'A';
   	}

  } //for
  
}

int
compare_keywords(const void *a,
                 const void *b) {
  return (strcmp(*((const char **)a), *((const char **)b)));
}


//
// 'style_init()' - Initialize the style buffer...
//

void
style_init(void) {
  char *style = new char[textbuf->length() + 1];
  char *text = textbuf->text();
  

  memset(style, 'A', textbuf->length());
  style[textbuf->length()] = '\0';

  if (!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());

  if(cppfile) style_parse(text, style, textbuf->length());

  stylebuf->text(style);
  delete[] style;
  free(text);
}


//
// 'style_unfinished_cb()' - Update unfinished styles.
//

void
style_unfinished_cb(int, void*) {
}


//
// 'style_update()' - Update the style buffer...
//

void
old_style_update(	int        pos,		// I - Position of update
             	int        nInserted,	// I - Number of inserted chars
	     		int        nDeleted,	// I - Number of deleted chars
             	int        /*nRestyled*/,	// I - Number of restyled chars
	     		const char * /*deletedText*/,// I - Text that was deleted
             	void       *cbArg) {	// I - Callback data
  int	start,				// Start of text
	end;				// End of text
  char	last,				// Last style on line
	stringdeleted=0;
  char *style,				// Style data
	*text;				// Text data

/*
  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    stylebuf->unselect();
    return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = (char*)malloc(nInserted + 1);
    memset(style, 'A', nInserted);
    style[nInserted] = '\0'; 

    stylebuf->replace(pos, pos + nDeleted, style);
    free(style);
  } else {
    // Just delete characters in the style buffer...
    if((stylebuf->character(pos) == 'D') || (stylebuf->character(pos) == 'C')) stringdeleted = 1;
    
    stylebuf->remove(pos, pos + nDeleted);
    if(pos < 2) style_init();
  }

	 
	
  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the line of the changed region to the end of
  // the line of the changed region...  Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = textbuf->line_start(pos);
  end   = textbuf->line_end(pos + nInserted);
  text  = textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  last  = style[end - start - 1];


  style_parse(text, style, end - start);


  stylebuf->replace(start, end, style);
  ((Fl_Text_Editor_ext *)cbArg)->redisplay_range(start, end);

  if ((last != style[end - start - 1]) || nDeleted || style[end - start - 1] == 'D') 
  if (last == 'C' || last == 'D') 
  //if(update_count > 10)
  {
    // The last character on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);

    start = 0;
    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);

    style_parse(text, style, end - start);
    
  	//if(update_count > 10)
  	{
  		update_count = 0;
  		if(browser_nav_grp->visible()) navigator_update(text,style,end - start);
  	} 

    stylebuf->replace(start, end, style);
    //((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
    
	te->redraw();
  }
  update_count+= nInserted > nDeleted ? nInserted : nDeleted;

  free(text);
  free(style);
*/
}


void
style_update(	int        pos,		// I - Position of update
             	int        nInserted,	// I - Number of inserted chars
	     		int        nDeleted,	// I - Number of deleted chars
             	int        /*nRestyled*/,	// I - Number of restyled chars
	     		const char * /*deletedText*/,// I - Text that was deleted
             	void       *cbArg) {	// I - Callback data
  int	start,				// Start of text
	end;				// End of text
  char	last,				// Last style on line
	stringdeleted=0;
  char *style,				// Style data
	*text;				// Text data


  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    stylebuf->unselect();
    return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = (char*)malloc(nInserted + 1);
    memset(style, 'A', nInserted);
    style[nInserted] = '\0'; 

    stylebuf->replace(pos, pos + nDeleted, style);
    free(style);
  } else {
    // Just delete characters in the style buffer...
//    if((stylebuf->character(pos) == 'D') || (stylebuf->character(pos) == 'C')) stringdeleted = 1;
    
    stylebuf->remove(pos, pos + nDeleted);
    if(pos < 2) style_init();
  }

	 
	
  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the line of the changed region to the end of
  // the line of the changed region...  Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = textbuf->line_start(pos);
  end   = textbuf->line_end(pos + nInserted);
  text  = textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  last  = style[end - start - 1];


  style_parse(text, style, end - start);


  stylebuf->replace(start, end, style);
  ((Fl_Text_Editor_ext *)cbArg)->redisplay_range(start, end);

  //if ((last != style[end - start - 1]) || nDeleted || style[end - start - 1] == 'D') 
  //if(update_count > 10)
  {
    // The last character on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);

    start = 0;
    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);

    style_parse(text, style, end - start);
    
	add_nav_timeout_handler();
  	//if(update_count > 10)
  	{
  		//update_count = 0;
  		//if(browser_nav_grp->visible()) navigator_update(text,style,end - start);
  	} 

    stylebuf->replace(start, end, style);
    //((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
    
	te->redraw();
  }
  //update_count+= nInserted > nDeleted ? nInserted : nDeleted;

  free(text);
  free(style);
}




