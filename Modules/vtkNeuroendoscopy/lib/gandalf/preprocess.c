/**
 * File:          $RCSfile: preprocess.c,v $
 * Module:        Preprocess source code for documentation generation
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:10:15 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define MAXLINESIZE 10000

/* file type */
typedef enum { PPCFILE, PPHFILE }
PPFileType;
   
#define CFILE 0
#define HFILE 1

/* Boolean values */
typedef enum { PPFALSE=0, PPTRUE=1 }
PPBoolean;

/* structure type for definition list */
typedef struct PPDefinition
{
   char *name, *def;
   struct PPDefinition *next;
} PPDefinition;

/* global list of definitions */
static PPDefinition *define_list = NULL;

/* structure type for #ifdef stack */
typedef struct PPIfdefState
{
   PPBoolean understood;
   PPBoolean echo;  /* whether to echo input */
   PPBoolean parse; /* whether to use definitions in block */
   struct PPIfdefState *next;
} PPIfdefState;

#if 0
static void define_list_print(void)
{
   PPDefinition *definition;

   for ( definition = define_list; definition != NULL;
         definition = definition->next )
      printf ( "Definition %s --> %s\n", definition->name, definition->def );
}
#endif

static void define_list_add ( char *name, char *def )
{
   PPDefinition *new = (PPDefinition *) malloc(sizeof(PPDefinition));

   assert ( new != NULL );

   /* copy strings */
   new->name = (char *) malloc((strlen(name)+1)*sizeof(char));
   assert ( new->name != NULL );
   strcpy ( new->name, name );
   if ( def == NULL )
      new->def = NULL;
   else
   {
      new->def  = (char *) malloc((strlen(def)+1)*sizeof(char));
      assert ( new->def != NULL );
      strcpy ( new->def,  def  );
   }
   
   /* add to list */
   new->next = define_list;
   define_list = new;
}

static void define_list_remove ( char *name )
{
   PPDefinition *definition, *prev;

   for ( prev = NULL, definition = define_list; definition != NULL;
         prev = definition, definition = definition->next )
      if ( strcmp ( definition->name, name ) == 0 )
         break;

   if ( definition != NULL )
   {
      /* remove definition from list */
      if ( prev == NULL ) define_list = definition->next;
      else prev->next = definition->next;

      if ( definition->def != NULL ) free ( definition->def );
      free ( definition->name );
      free ( definition );
   }
}

static PPBoolean define_list_test ( char *name )
{
   PPDefinition *definition;

   for ( definition = define_list; definition != NULL;
         definition = definition->next )
      if ( strcmp ( definition->name, name ) == 0 )
         return PPTRUE;
         
   return PPFALSE;
}

static char *define_list_get ( char *name )
{
   PPDefinition *definition;

   for ( definition = define_list; definition != NULL;
         definition = definition->next )
      if ( strcmp ( definition->name, name ) == 0 )
         return definition->def;
         
   return NULL;
}

static void parse_file ( char *directory, char *input_file, FILE *fpout,
                         PPFileType type, PPBoolean top_level,
                         PPBoolean write_output )
{
   char line[MAXLINESIZE], s1[1000], s2[1000], s3[1000];
   int fend;
   FILE *fpin;
   PPIfdefState *state=NULL;

   /* open input file */
   if ( top_level )
      fpin = fopen ( input_file, "r" );
   else
   {
      char long_name[1000];

      sprintf ( long_name, "%s/%s", directory, input_file );
      fpin = fopen ( long_name, "r" );
   }
   
   if ( fpin == NULL )
   {
      fprintf ( stderr, "Error: cannot open input file %s\n", input_file );
      exit(EXIT_FAILURE);
   }
   
   while ( fgets ( line, MAXLINESIZE, fpin ) != NULL )
   {
      /* check for #include line */
      if ( sscanf ( line, "#include <%s", s1 ) == 1 )
      {
         /* ignore if current state is to ignore */
         if ( state != NULL && !state->parse ) continue;

         /* locate > at end of file name */
         for ( fend = strlen(s1)-1; fend >= 0; fend-- )
            if ( s1[fend] == '>' ) break;

         if ( fend < 0 )
         {
            fprintf ( stderr, "Corrupted include line\n" );
            exit(EXIT_FAILURE);
         }

         /* terminate file name */
         s1[fend] = '\0';

         /* ignore non-Gandalf header files */
         if ( strncmp ( s1, "gandalf", 7 ) != 0 )
         {
            /* normal include line */
            if ( write_output && (state == NULL || state->parse) )
               fputs ( line, fpout );
            continue;
         }

         if ( type == CFILE )
         {
            if ( strncmp ( "_noc.c", &s1[fend-6], 6 ) != 0 &&
                 strncmp ( "_noc.h", &s1[fend-6], 6 ) != 0 )
            {
               /* normal include line */
               if ( write_output && (state == NULL || state->parse) )
                  fputs ( line, fpout );

               /* parse file for definitions but without adding to output */
               if ( strncmp ( ".c", &s1[fend-2], 2 ) == 0 )
                  parse_file ( directory, s1, fpout, PPCFILE,
                               PPFALSE, PPFALSE );
               else if ( strncmp ( ".h", &s1[fend-2], 2 ) == 0 )
                  parse_file ( directory, s1, fpout, PPHFILE,
                               PPFALSE, PPFALSE );
               else assert(0);
                  
               continue;
            }
         }
         else if ( type == HFILE )
         {
            if ( strncmp ( "_noc.h", &s1[fend-6], 6 ) != 0 )
            {
               /* normal include line */
               if ( write_output && (state == NULL || state->parse) )
                  fputs ( line, fpout );

               /* parse file for definitions but without adding to output */
               parse_file ( directory, s1, fpout, PPHFILE,
                            PPFALSE, PPFALSE );
               continue;
            }
         }
         else
         {
            fprintf ( stderr, "illegal type %d\n", type );
            exit(EXIT_FAILURE);
         }
      
         /* strip > from end of string and recursively include file */
         parse_file ( directory, s1, fpout, type, PPFALSE, write_output );
      }
      /* check for #define line with two values */
      else if ( sscanf ( line, "#define %s %s %s", s1, s2, s3 ) == 3 )
      {
         /* ignore if current state is to ignore */
         if ( state != NULL && !state->parse ) continue;

         /* concatenate strings */
         strcat ( s2, " " );
         strcat ( s2, s3 );
         
         /* check that #define declaration has GAN_ prefix */
         if ( strlen(s1) > 4 && strncmp ( s1, "GAN_", 4 ) == 0 )
         {
            /* check whether this is a macro with arguments */
            for ( fend = strlen(s1)-1; fend >= 0; fend-- )
               if ( s1[fend] == ')' ) break;

            if ( fend < 0 )
               /* add definition to list */
               define_list_add ( s1, s2 );

            /* echo line to output */
            if ( write_output ) fputs ( line, fpout );
         }
         /* check that #define declaration has _GAN prefix */
         else if ( strlen(s1) > 4 && strncmp ( s1, "_GAN", 4 ) == 0 )
         {
            /* check whether this is a macro with arguments */
            for ( fend = strlen(s1)-1; fend >= 0; fend-- )
               if ( s1[fend] == ')' ) break;

            if ( fend < 0 )
               /* add definition to list */
               define_list_add ( s1, s2 );

            /* echo line to output */
            if ( write_output ) fputs ( line, fpout );
         }
         else
         {
            /* treat as normal line of code */
            if ( write_output ) fputs ( line, fpout );

            continue;
         }
      }
      /* check for #define line with value */
      else if ( sscanf ( line, "#define %s %s", s1, s2 ) == 2 )
      {
         /* ignore if current state is to ignore */
         if ( state != NULL && !state->parse ) continue;

         /* check that #define declaration has GAN_ prefix */
         if ( strlen(s1) > 4 && strncmp ( s1, "GAN_", 4 ) == 0 )
         {
            /* check whether this is a macro with arguments */
            for ( fend = strlen(s1)-1; fend >= 0; fend-- )
               if ( s1[fend] == ')' ) break;

            if ( fend < 0 )
               /* add definition to list */
               define_list_add ( s1, s2 );

            /* echo line to output */
            if ( write_output ) fputs ( line, fpout );
         }
         /* check that #define declaration has _GAN prefix */
         else if ( strlen(s1) > 4 && strncmp ( s1, "_GAN", 4 ) == 0 )
         {
            /* check whether this is a macro with arguments */
            for ( fend = strlen(s1)-1; fend >= 0; fend-- )
               if ( s1[fend] == ')' ) break;

            if ( fend < 0 )
               /* add definition to list */
               define_list_add ( s1, s2 );

            /* echo line to output */
            if ( write_output ) fputs ( line, fpout );
         }
         else
         {
            /* treat as normal line of code */
            if ( write_output ) fputs ( line, fpout );

            continue;
         }
      }
      /* check for #define line without value */
      else if ( sscanf ( line, "#define %s", s1 ) == 1 )
      {
         /* ignore if current state is to ignore */
         if ( state != NULL && !state->parse ) continue;

         /* check whether #define declaration has GAN_ prefix */
         if ( strlen(s1) > 4 && (strncmp ( s1, "GAN_", 4 ) == 0 ||
                                 strncmp ( s1, "_GAN", 4 ) == 0) )
         {
            /* add definition to list */
            define_list_add ( s1, NULL );

            /* echo line */
            if ( write_output ) fputs ( line, fpout );
         }
         else
         {
            /* treat as normal line of code */
            if ( write_output ) fputs ( line, fpout );

            continue;
         }
      }
      /* check for #undef line */
      else if ( sscanf ( line, "#undef %s", s1 ) == 1 )
      {
         /* ignore if current state is to ignore */
         if ( state != NULL && !state->parse ) continue;

         /* check whether #undef declaration has GAN_ prefix */
         if ( strlen(s1) > 4 && strncmp ( s1, "GAN_", 4 ) == 0 )
            /* remove definition from list */
            define_list_remove ( s1 );

         /* echo to output */
         if ( write_output ) fputs ( line, fpout );

         continue;
      }
      /* check for #ifdef line */
      else if ( sscanf ( line, "#ifdef %s", s1 ) == 1 )
      {
         PPIfdefState *new = (PPIfdefState *) malloc(sizeof(PPIfdefState));

         assert(new != NULL);

         /* check whether #ifdef test symbol has GAN_ prefix */
         if ( strlen(s1) > 4 && strncmp ( s1, "GAN_", 4 ) == 0 )
         {
            /* treat as non-Gandalf #ifdef if current state is ignore */
            if ( state != NULL && !state->parse )
            {
               new->understood = PPFALSE;
               new->echo = PPFALSE;
               new->parse = PPFALSE;
            }
            else
            {
               new->understood = PPTRUE;
               new->echo = PPFALSE;
               new->parse = define_list_test(s1);
            }
         }
         else if ( strlen(s1) > 4 && strncmp ( s1, "_GAN", 4 ) == 0 )
         {
            /* treat as non-Gandalf #ifdef if current state is ignore */
            if ( state != NULL && !state->parse )
            {
               new->understood = PPFALSE;
               new->echo = PPFALSE;
               new->parse = PPFALSE;
            }
            else
            {
               new->understood = PPTRUE;
               new->echo = PPTRUE;
               new->parse = define_list_test(s1);

               /* output as normal line of code */
               if ( write_output && new->parse ) fputs ( line, fpout );
            }
         }
         else
         {
            new->understood = PPFALSE;
            new->parse = (state == NULL) ? PPTRUE : state->parse;
            new->echo = new->parse;

            /* output as normal line of code */
            if ( write_output && new->parse ) fputs ( line, fpout );
         }
         
         /* add #ifdef to stack */
         new->next = state;
         state = new;
      }
      /* check for #ifndef line */
      else if ( sscanf ( line, "#ifndef %s", s1 ) == 1 )
      {
         PPIfdefState *new = (PPIfdefState *) malloc(sizeof(PPIfdefState));

         assert(new != NULL);

         /* check whether #ifndef test symbol has GAN_ prefix */
         if ( strlen(s1) > 4 && strncmp ( s1, "GAN_", 4 ) == 0 )
         {
            /* treat as non-Gandalf #ifndef is current state is ignore */
            if ( state != NULL && !state->parse )
            {
               new->understood = PPFALSE;
               new->echo = PPFALSE;
               new->parse = PPFALSE;
            }
            else
            {
               new->understood = PPTRUE;
               new->echo = PPFALSE;
               new->parse = define_list_test(s1) ? PPFALSE : PPTRUE;
            }
         }
         /* check whether #ifndef test symbol has _GAN_ prefix */
         else if ( strlen(s1) > 4 && strncmp ( s1, "_GAN", 4 ) == 0 )
         {
            /* treat as non-Gandalf #ifndef is current state is ignore */
            if ( state != NULL && !state->parse )
            {
               new->understood = PPFALSE;
               new->echo = PPFALSE;
               new->parse = PPFALSE;
            }
            else
            {
               new->understood = PPTRUE;
               new->echo = PPTRUE;
               new->parse = define_list_test(s1) ? PPFALSE : PPTRUE;
               if ( write_output && new->parse ) fputs ( line, fpout );
            }
         }
         else
         {
            new->understood = PPFALSE;
            new->parse = (state == NULL) ? PPTRUE : state->parse;
            new->echo = new->parse;

            /* output as normal line of code */
            if ( write_output && new->parse ) fputs ( line, fpout );
         }
         
         /* add #ifndef to stack */
         new->next = state;
         state = new;
      }
      /* check for #if line */
      else if ( sscanf ( line, "#if %s", s1 ) == 1 )
      {
         PPIfdefState *new = (PPIfdefState *) malloc(sizeof(PPIfdefState));

         assert(new != NULL);

         /* we don't understand #if's apart from #if 0 and #if 1 */
         if ( strcmp ( s1, "0" ) == 0 )
         {
            new->understood = PPFALSE;
            new->echo = PPFALSE;
            new->parse = PPFALSE;
         }
         else
         {
            new->understood = PPFALSE;
            new->echo = (state == NULL) ? PPTRUE : state->parse;
            new->parse = (state == NULL) ? PPTRUE : state->parse;
         }
         
         /* output as normal line of code */
         if ( write_output && new->echo ) fputs ( line, fpout );
         
         /* add #ifdef to stack */
         new->next = state;
         state = new;
      }
      /* check for #else line */
      else if ( strncmp ( line, "#else", 5 ) == 0 )
      {
         /* if current #ifdef state is understood, swap the sense of it */
         assert ( state != NULL );
         if ( write_output && state->echo ) fputs ( line, fpout );
         if ( state->understood )
            state->parse = (state->parse) ? PPFALSE : PPTRUE;
      }
      /* check for #elif line */
      else if ( strncmp ( line, "#elif", 5 ) == 0 )
      {
         /* can't handle #elif properly, so enforce its incomprehensibility */
         assert ( state != NULL && !state->understood );
         if ( write_output && state->echo )
            fputs ( line, fpout );
      }
      /* check for #endif */
      else if ( strncmp ( line, "#endif", 6 ) == 0 )
      {
         PPIfdefState *latest = state;

         /* pop the latest #endif from the stack */
         assert ( state != NULL );
         if ( write_output &&
              (state->echo &&
               strcmp (line, "#endif /* #ifdef GAN_GENERATE_DOCUMENTATION */")
               != 0) )
            fputs ( line, fpout );

         state = state->next;
         free(latest);
      }
      else
      {
         /* normal line of code */
         if ( write_output && (state == NULL || state->parse) )
         {
            char *gotit;
            char *def;

            /* check for start of comment block */
            if ( line[0] == '/' && line[1] == '*' && line[2] == '*' )
            {
               fprintf ( fpout, "/**\n" );
               continue;
            }
            /* check for end of comment block */
            else if ( strlen(line) >= 4 && line[strlen(line)-4] == '*' &&
                      line[strlen(line)-3] == '*' &&
                      line[strlen(line)-2] == '/' )
            {
               fprintf ( fpout, " */\n" );
               continue;
            }
            /* check for file definition line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* File: $RCSfile: %s", s1 ) == 1 ||
                       sscanf ( line, " * File: $RCSfile: %s", s1 ) == 1) )
            {
               /* cut off ,v suffix */
               if ( strlen(s1) < 2 )
               {
                  fprintf ( stderr, "Corrupted file definition line\n" );
                  exit(EXIT_FAILURE);
               }

               /* output file line */
               s1[strlen(s1)-2] = '\0';
               fprintf ( fpout, " * \\file %s\n", s1 );
               continue;
            }
            /* check for author definition line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* Author: $Author: %s", s1 ) == 1 ||
                       sscanf ( line, " * Author: $Author: %s", s1 ) == 1) )
            {
               /* output author line */
               fprintf ( fpout, " * \\author %s\n", s1 );
               continue;
            }
            /* check for revision definition line if at top level */
            else if ( top_level &&
                   (sscanf ( line, "* Revision: $Revision: %s", s1 ) == 1 ||
                    sscanf ( line, " * Revision: $Revision: %s", s1 ) == 1) )
            {
               /* output version line */
               fprintf ( fpout, " * \\version %s\n", s1 );
               continue;
            }
            /* check for date line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* Last edited: $Date: %s %s", s1, s2 ) == 2 ||
                       sscanf ( line, " * Last edited: $Date: %s %s", s1, s2 ) == 2) )
            {
               /* output date line */
               fprintf ( fpout, " * \\date %s %s\n", s1, s2 );
               continue;
            }
            /* check for module line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* %s", s1 ) == 1 ||
                       sscanf ( line, " * %s", s1 ) == 1) &&
                      strcmp ( s1, "Module:" ) == 0 )
            {
               /* output module line */
               fprintf ( fpout, " * Module: %s *\n", &line[10] );
               continue;
            }
            /* check for notes line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* %s", s1 ) == 1 ||
                       sscanf ( line, " * %s", s1 ) == 1) &&
                      strcmp ( s1, "Notes:" ) == 0 )
            {
               /* output module line */
               fprintf ( fpout, " * \\note %s", &line[8] );
               continue;
            }
            /* check for history line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* %s", s1 ) == 1 ||
                       sscanf ( line, " * %s", s1 ) == 1) &&
                      strcmp ( s1, "History:" ) == 0 )
            {
               /* output module line */
               fprintf ( fpout, " * \\history %s", &line[11] );
               continue;
            }
            /* check for warning line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* %s", s1 ) == 1 ||
                       sscanf ( line, " * %s", s1 ) == 1) &&
                      strcmp ( s1, "Warnings:" ) == 0 )
            {
               /* output warning line */
               fprintf ( fpout, " * \\warning %s", &line[12] );
               continue;
            }
            /* check for copyright line if at top level */
            else if ( top_level &&
                      (sscanf ( line, "* %s", s1 ) == 1 ||
                       sscanf ( line, " * %s", s1 ) == 1) &&
                      strcmp ( s1, "Copyright:" ) == 0 )
            {
               /* output copyright line */
               fprintf ( fpout, " *\n * Copyright: %s", &line[13] );
               continue;
            }

            // initialise counter on line
            fend = 0;

            /* check for GANDALF_API in the line */
            if ( (gotit = strstr ( line, "GANDALF_API" )) != NULL )
               fend = (gotit-line) + strlen("GANDALF_API");

            /* search for Gandalf macros in this input line */
            while ( line[fend] != '\0' )
            {
               int j=0;

               if ( (gotit = strstr ( &line[fend], "GAN_" )) == NULL )
               {
                  /* output rest of string and exit loop */
                  fputs ( &line[fend], fpout );
                  break;
               }

               /* output string up to located string */
               while ( &line[fend] != gotit )
                  fputc ( line[fend++], fpout );

               /* copy symbol name character by character into string s1 */
               while ( isalnum(line[fend]) || line[fend] == '_' )
                  s1[j++] = line[fend++];

               /* terminate string s2 */
               s1[j] = '\0';

               /* if symbol is defined, replace it, otherwise copy it */
               def = define_list_get(s1);
               if ( def == NULL )
                  fputs ( s1, fpout );
               else
                  fputs ( def, fpout );
            }
         }

         continue;
      }
   }

   /* check for balanced #if's and #endif's */
   assert ( state == NULL );
   fclose(fpin);
}

static void process_file ( char *directory, char *file_name )
{
   FILE *fpout = NULL;
   int type;
   char tmpfile[] = "/tmp/h432";
#if 1
   char system_call[1000];
#endif
   /* open temporary output file */
   fpout = fopen ( tmpfile, "w" );
   if ( fpout == NULL )
   {
      fprintf ( stderr, "Error: cannot open output file %s\n", tmpfile );
      exit(EXIT_FAILURE);
   }
   
   /* determine file type */
   if ( file_name[strlen(file_name)-1] == 'c' )
      type = CFILE;
   else if ( file_name[strlen(file_name)-1] == 'h' )
      type = HFILE;
   else
   {
      fprintf ( stderr, "illegal file name %s\n", file_name );
      exit(EXIT_FAILURE);
   }
   
   /* build output file */
   parse_file ( directory, file_name, fpout, type, PPTRUE, PPTRUE );

   /* successful exit */
   fclose(fpout);
#if 1
   /* move file into temporary directory */
   sprintf ( system_call, "/bin/mv %s tmp/%s", tmpfile, file_name );
   if ( system(system_call) != 0 )
   {
      fprintf ( stderr, "Error: cannot move file %s to %s\n", tmpfile,
                file_name );
      exit(EXIT_FAILURE);
   }
#endif
}

int main ( int argc, char *argv[] )
{
   char system_call[1000];

   /* make tmp directory if necessary */
   sprintf ( system_call, "mkdir -p tmp" );
   system(system_call);
   
   while ( argc > 2 )
   {
      /* add definition to start with */
      define_list_add ( "GAN_GENERATE_DOCUMENTATION", NULL );

      process_file ( argv[1], argv[--argc] );

      /* clear definitions */
      while ( define_list != NULL )
         define_list_remove ( define_list->name );
   }
   
   return EXIT_SUCCESS;
}
