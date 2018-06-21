/* c2html
 * Ver.   : 0.0.5
 * Author : gynvael.coldwind//vx
 * Date   : 02.03.2003
 * Desc.  : converts a c file into a html document with colored syntax
 */

/* TODO list:
 - change the argument -parsing- stuff
 - add tag-type option
 - stdin and stdout support
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"keywords.h"
#include"header.h"
#include"footer.h"

/* 
 * defines
 */
#define MAXPATH 256
#ifdef EXPERIMENTAL
  #define VERSION "0.0.5-x"
#else
  #define VERSION "0.0.5"
#endif

/*
 * enums
 */
 
/* styles */
enum
{
  STYLE_NORMAL,   /* function names, variable names, etc  */
  STYLE_KEYWORD,  /* special names like "if" "int", etc   */
  STYLE_NUMBER,   /* digits (oct/dec/hex)                 */
  STYLE_COMMENTS, /* c-style and cpp-style comments       */
  STYLE_STRING,   /* "strings" and 'strings'              */
  STYLE_PREPROC,  /* #preprocessor stuff                  */
  STYLE_SYMBOL,   /* !@#$%^&*()-={}[];' etc               */
  STYLE_END       /* end of previous style, virtual style */
};

/* states for color_text func */
enum
{
  ST_SWITCH      = 0, /* decides what state is next, handles sp \n \r \t */
  ST_PREPROC     = 1, /* handles preprocessor line                       */
  ST_STRING      = 2, /* handles quotas and double quotas                */
  ST_COMMENTSC   = 3, /* handles c-style comments                        */
  ST_COMMENTSCPP = 4, /* handles cpp-style comments                      */
  ST_KEYWORD     = 5, /* handles keywords or switches to ST_NORMAL       */
  ST_NORMAL      = 6, /* handles normal alphanumeric strings             */
  ST_SYMBOL      = 7, /* handles non-alphanumeric non-control chars      */
  ST_NUMBER      = 8  /* handles numbers in oct/dec/hex format           */
};

/*
 * structs
 */
typedef struct st_tags
{
  const char *tag;
  int size;
} t_tags;

/*
 * global predefined structs
 */
 
/* <code> tag struct */
t_tags code_tags[ ] = 
{
  { "<code class=\"n\">",  16 }, /* normal       */
  { "<code class=\"k\">",  16 }, /* keyword      */
  { "<code class=\"no\">", 17 }, /* number       */
  { "<code class=\"c\">",  16 }, /* comments     */
  { "<code class=\"s\">",  16 }, /* string       */
  { "<code class=\"p\">",  16 }, /* preprocesor  */
  { "<code class=\"sy\">", 17 }, /* symbol       */
  { "</code>",              7 }, /* end of style */
};

#ifdef EXPERIMENTAL
/* <pre> tag struct */
t_tags pre_tags[ ] = 
{
  { "<pre class=\"n\">",   15 }, /* normal       */
  { "<pre class=\"k\">",   15 }, /* keyword      */
  { "<pre class=\"no\">",  16 }, /* number       */
  { "<pre class=\"c\">",   15 }, /* comments     */
  { "<pre class=\"s\">",   15 }, /* string       */
  { "<pre class=\"p\">",   15 }, /* preprocesor  */
  { "<pre class=\"sy\">",  16 }, /* symbol       */
  { "</pre>",               6 }  /* end of style */
};
#else
 #define pre_tags (void*)0
#endif

/*
 * globals
 */
 
/* tag struct pointer */
t_tags *tags = code_tags; /* <code> is default */

/*
 * functions
 */

/* color_text( )
 * sets up styles for the text (c/cpp syntax)
 * arguments: char *text - pointer to buffer with text to set styles to
 *            char *style - pointer to buffer to write styles to
 * returns: 0
 */
int
color_text( char *text, char *style )
{
  int state = ST_SWITCH;        /* current state                     */
  int old_style = STYLE_NORMAL; /* old style (for sp & \n & \t opt.) */
  char temp, tmp;               /* some temp vars used in states     */
  char kbuf[ 16 ], *pnow;       /* keywords temp buffer              */
  int res, lkey, rkey, pos;     /* keywords binsearch vars           */
  
  /* while there still is some text */
  while( *text )
  {
    /* go to the proper state */
    switch( state )
    {
      /* state: ST_SWITCH 
       * desc : switches to the proper state and handles \r \n \t and sp
       */
      case ST_SWITCH:
      if( *text == '#' ) { state = ST_PREPROC; }
      else if( *text == '/' && *(text+1) == '*' ) { state = ST_COMMENTSC; }
      else if( *text == '/' && *(text+1) == '/' ) { state = ST_COMMENTSCPP; }
      else if( *text == '\"' || *text == '\'' ) { state = ST_STRING; }
      else if( *text >= '0' && *text <= '9' ) { state = ST_NUMBER; }
      else if( *text >= 'A' && *text <= 'Z' ) { state = ST_KEYWORD; }
      else if( *text >= 'a' && *text <= 'z' ) { state = ST_KEYWORD; }
      else if( *text == '_' ) { state = ST_KEYWORD; }
      else if( *text != '\r' && *text != '\n' &&
               *text != '\t' && *text != ' ' ) { state = ST_SYMBOL; }
      else
      {
        /* handle \r \n \t and sp using old_style */
        *style = old_style;
        style++;
        text++;
      }
      break;
      
      /* state: ST_PREPROC
       * desc : handles preprocessors lines (from # to eoln)
       */
      case ST_PREPROC:
      old_style = STYLE_PREPROC;
      while( *text && *text != '\n' )
      {
        /* set it's style */
        *style = STYLE_PREPROC;
        style++;
        text++;
      }
      state = ST_SWITCH;      
      break;      
      
      /* state: ST_STRING 
       *  desc: handles strings (from " to " and from ' to ')
       */
      case ST_STRING:
      old_style = STYLE_STRING;
      temp = *text; /* remember the char that started the string (' ") */
      tmp = 1; /* if 0, the last char was a backslash */
      
      /* the first char is a string for sure, get rid of it */
      *style = STYLE_STRING; 
      text++;
      style++;
      
      /* while we don't hit the char that started this state
       * or it's a not-important char (backslash in front) */
      while( *text && (!( *text == temp && tmp )) )
      {
        /* check if we have a backslash or not */
        if( *text == '\\' && tmp ) tmp = 0; else tmp = 1;
        
        /* set the tyle */
        *style = STYLE_STRING;
        text++;
        style++;
      }
      
      /* set the style to the closing (double) quote */
      if( *text )
      {
        *style = STYLE_STRING;
        text++;
        style++;
      }
      
      state = ST_SWITCH;
      break;      
      
      /* state: ST_COMMENTSC
       * desc : handles c-style comments ( from (slash)* to *(slash) )
       */
      case ST_COMMENTSC:
      old_style = STYLE_COMMENTS;
      tmp = 0; /* end of comment flag */
      temp = 0; /* last-char-was-a-asterisk flag */
      
      /* the first two chars are comments for sure, get rid of them */
      *style = STYLE_COMMENTS;
      *(style+1) = STYLE_COMMENTS;
      style+=2;
      text+=2;      
      
      /* while we still have text and we didn't hit the end of comment */
      while( *text && !tmp )
      {
        /* check for end of comment marker */
        if( *text == '/' && temp ) tmp = 1;
        
        /* check for a asterisk */
        if( *text == '*' ) temp = 1; else temp = 0;
        
        /* set the style */
        *style = STYLE_COMMENTS;
        style++;
        text++;
      }
      
      state = ST_SWITCH;      
      break;      
      
      /* state: ST_COMMENTSCPP
       * desc : handles cpp-style comments ( from // to eoln )
       */
      case ST_COMMENTSCPP:
      /* while we still have text and it's not a eoln */
      while( *text && *text != '\n' )
      {
        /* set style */
        *style = STYLE_COMMENTS;
        style++;
        text++;        
      }
      
      old_style = STYLE_COMMENTS;
      state = ST_SWITCH;      
      break;      
      
      /* state: ST_KEYWORD
       * desc : handles keywords or jumps to ST_NORMAL state
       */
      case ST_KEYWORD:
      
      /* get the next word to a buffer */
      tmp = 0; /* letter count */
      pnow = text; /* local pointer to text */
     
      /* while we still have text that is alphanumeric */
      while( *pnow && 
             tmp < 11 &&
             ( ( *pnow >= 'A' && *pnow <= 'Z' ) ||
               ( *pnow >= 'a' && *pnow <= 'z' ) ||
               ( *pnow >= '0' && *pnow <= '9' ) ||
               ( *pnow == '_' ) ) )
      {
        /* write the chars to temp buffer */
        kbuf[ (int)tmp++ ] = *(pnow++);
      }
      
      /* if word size is greater then 10, cyah */
      if( tmp == 11 )
      {
        state = ST_NORMAL;
        break;
      }
            
      /* find out if the word is a keyword (binsearch) */
      kbuf[ (int)tmp ] = '\0';
      lkey = 0; rkey = 63; temp = -1; pos = -1;
      
      /* binary search */
      while( lkey < rkey )
      {
        if( pos == ( lkey+rkey ) / 2 ) break;
        pos = ( lkey+rkey ) / 2;
        res = strcmp( keywords[ pos ], kbuf );
        if( res == 0 ) 
        {
          temp = 1;
          break;
        }
        else if( res < 0 ) lkey = pos;
        else rkey = pos;
      }      
      
      /* is not a keyword? cyah */
      if( temp == -1 )
      {
        state = ST_NORMAL;
      
        break;
      }
      
      /* is? change it's style */      
      for( lkey = 0; lkey < tmp; lkey ++ )
      {
        *(style++) = STYLE_KEYWORD;
      }
      text = pnow;
      state = ST_SWITCH;
      old_style = STYLE_KEYWORD;
      break;      
      
      /* state: ST_NORMAL
       * desc : handles normal alphamnumeric words
       */
      case ST_NORMAL:
      /* while is alphanumeric */
      while( ( *text >= 'A' && *text <= 'Z' ) ||
             ( *text >= 'a' && *text <= 'z' ) ||
             ( *text >= '0' && *text <= '9' ) ||
             ( *text == '_' ) )
      {
        /* set it's style */
        *style = STYLE_NORMAL;
        style++;
        text++;
      }
      old_style = STYLE_NORMAL;
      state = ST_SWITCH;
      break;      
      
      /* state: ST_SYMBOL
       * desc : handles a non-alphanumeric symbol
       */
      case ST_SYMBOL:
      /* set it's style and exit */
      *style = STYLE_SYMBOL;
      style++;
      text++;
      state = ST_SWITCH;
      old_style = STYLE_SYMBOL;
      break;      

      /* state: ST_NUMBER
       * desc : handles numbers
       */
      case ST_NUMBER:
      /* while is a number 
       * (warning: no advanced check for hex vals, because that is
       * not needed, this is a syntax coloring function, not a
       * error checker)
       */
      while( ( *text >= 'A' && *text <= 'F' ) ||
             ( *text >= 'a' && *text <= 'f' ) ||
             ( *text >= '0' && *text <= '9' ) ||
             ( *text == 'x' || *text == 'X'  ) )
      {
        /* set it's style */
        *style = STYLE_NUMBER;
        style++;
        text++;
      }
      old_style = STYLE_NUMBER;
      state = ST_SWITCH;
      break;      
    }
  }
  
  /* cyah */
  return 0;
} /* color_text( ) */

/* info( )
 * prints info about the program
 */
void
info( void )
{
  printf( "c2html v." VERSION " (" __TIME__ " " __DATE__ 
          ") by gynvael.coldwind//vx\n"
          "  Desc: converts a c/cpp file into a html document with"
          " colored syntax\n"
          #ifdef EXPERIMENTAL
          "  Usage: c2html infile [outfile] [--pre|--code] [--with-header]\n"
          #else
          "  Usage: c2html infile [outfile] [--with-header]\n"
          #endif
          "    infile        - input file to color\n"
          "    outfile       - output file (default: infile.html)\n"
          #ifdef EXPERIMENTAL
          "    --pre         - use <pre> tag\n"
          "    --code        - use <code> tag (default)\n"
          #endif
          "    --with-header - add html header with styles\n\n"
          "  Example: c2html main.c main_c.html --with-header\n"
          );  
} /* info( ) */

/* load_file( )
 * creates the text & style arrays, loads the file to text array
 * arguments: const char *fname - input file name
 *            char **text - pointer to pointer to the text buffer
 *            char **style - pointer to pointer to the style buffer
 * returns: 0 - success
 *          1 - could not open the file
 *          2 - could not allocate memory
 *          3 - could not read from file
 */
int
load_file( const char *fname, char **text, char **style )
{
  FILE *f; /* file */
  int size, ret; /* size of file and ret var */
  
  /* open file */
  f = fopen( fname, "rb" );
  if( !f ) return 1;
  
  /* get file size */
  fseek( f, 0, SEEK_END );
  size = ftell( f );
  fseek( f, 0, SEEK_SET );
  
  /* alloc mem */
  *text = (char*) malloc( size + 1 );
  if( !*text )
  {
    fclose( f ); 
    return 2;
  }
  *style = (char*) malloc( size + 1 );
  if( !*style )
  {
    free( *text );
    fclose( f ); 
    return 2;
  }
  
  /* read from file */
  ret = fread( *text, 1, size, f );
  if( ret != size )
  {
    free( *text );
    free( *style );
    fclose( f );
    return 3;
  }
  (*text)[ size ] = '\0';
    
  fclose( f );
  return 0;  
} /* load_file( ) */

/* write_file( )
 * writes the output html file, destroys text and style array
 * arguments: const char *fname - output file name
 *            char **text - pointer to pointer to the text buffer
 *            char **style - pointer to pointer to the style buffer
 *            int head - 1 - write html header and footer
 * returns: 0 - success
 *          1 - error
 */
int
write_file( const char *fname, char **text, char **style, int head )
{
  FILE *f; /* file */
  char *atext = *text; /* text pointer */
  char *astyle = *style; /* style pointer */
  char *t = atext, *s = astyle; /* pointers */
  int current_style, soft_space = 1; /* current style and space flag */
  
  /* open the output file */
  f = fopen( fname, "wb" );
  if( !f )
  {
    return 1;
  }
  
  /* write header */
  if( head )
  {
    fwrite( html_header, 1, sizeof( html_header ) - 1, f );
  }
  
  /* write the data while there is still some text */
  current_style = -1;
  while( *t )
  {
    /* if new style comes */
    if( *s != current_style )
    {
      /* and it's not the "start" style */
      if( current_style != -1 )
      {
        /* write the end tag */
        fwrite( tags[ STYLE_END ].tag, 1, tags[ STYLE_END ].size, f );
      }
      /* write the new style tag */
      fwrite( tags[ (int)*s ].tag, 1, tags[ (int)*s ].size, f );
      current_style = *s;
    }
    
    /* check for specials */
    switch( *t )
    {
      /* ignore \r */
      case '\r': break;    
      
      /* handle \n */
      #ifndef LINUX
      case '\n':
      if( tags != pre_tags )
      {
        soft_space = 1;
        fwrite( "<br/>\r\n", 1, 7, f ); 
        break;
      }
      fwrite( "\r\n", 1, 2, f ); break;
      #else
      case '\n': 
      if( tags != pre_tags )
      {
        soft_space = 1;
        fwrite( "<br/>\n", 1, 6, f ); 
        break;
      }
      fwrite( "\n", 1, 1, f ); break;
      #endif
      
      /* switch < > and & for html entieties */
      case '<': fwrite( "&lt;", 1, 4, f ); soft_space = 0; break;
      case '>': fwrite( "&gt;", 1, 4, f ); soft_space = 0; break;
      case '&': fwrite( "&amp;", 1, 5, f ); soft_space = 0; break;
      
      /* handle space */
      case ' ': 
      if( tags != pre_tags && soft_space )
      { 
        fwrite( "&nbsp;", 1, 6, f );
      }
      else
      {
        fwrite( " ", 1, 1, f ); 
      }
      soft_space = !soft_space;
      break;
      
      /* hansle tab */
      case '\t': 
      if( tags != pre_tags )
      {
        fwrite( "&nbsp; &nbsp; &nbsp; &nbsp; ",
                         1, 28, f ); soft_space = 1; 
        break;
      }
      fwrite( "        ", 1, 8, f );
      break;       
                         
      /* write any other char */
      default: fwrite( t, 1, 1, f ); soft_space = 0;
    }
    
    /* next */
    t++; s++;
  }
  
  /* write the end tag */
  if( current_style != -1 )
  {
    fwrite( tags[ STYLE_END ].tag, 1, tags[ STYLE_END ].size, f );
  }
  
  /* write footer */
  if( head )
  {
    fwrite( html_footer, 1, sizeof( html_footer ) - 1, f );
  }
  
  /* close and free */
  fclose( f );
  free( atext );
  free( astyle );
  text = NULL;
  style = NULL;
  return 0;  
} /* write_file( ) */

/* main( ) */
int
main( int argc, char **argv )
{
  char infile_name[ MAXPATH ], outfile_name[ MAXPATH ];
  char *arr_text = NULL,  /* text array  */
       *arr_style = NULL; /* style array */
  int ret, head = 0; /* ret var */
  
  /* if less then 1 arg */
  if( argc < 2 ) 
  {
    info( );
    return 0;
  } 
  /* if 2 or more */
  else
  {
    strncpy( infile_name, argv[ 1 ], MAXPATH );
  }
  
  /* if 3 or more */
  if( argc >= 3 ) 
  {
    strncpy( outfile_name, argv[ 2 ], MAXPATH );
  } 
  else
  {
    strncpy( outfile_name, argv[ 1 ], MAXPATH );
    strncat( outfile_name, ".html", MAXPATH );
  }
  
  /* if 4 (temp) */
  if( argc == 4 )
  {
    if( strcmp( "--with-header", argv[ 3 ] ) == 0 )
    {
      head = 1;      
    }
  }
  
  /* load file */
  ret = load_file( infile_name, &arr_text, &arr_style );
  if( ret != 0 )
  {
    switch( ret )
    {
      case 1: printf("Could not open file %s\n", infile_name); break;
      case 2: puts("Could not allocate memory"); break;
      case 3: printf("Could not read from file %s\n", infile_name); break;
      default: puts("Unknown error"); break;
    }
    return 1;
  }
  
  /* color text */
  color_text( arr_text, arr_style );
  
  /* write file */
  write_file( outfile_name, &arr_text, &arr_style, head );
  
  
  return 0;  
}


