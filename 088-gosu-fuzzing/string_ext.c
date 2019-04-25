/* String Stuff That Helps Parsing =^^=
 * Ver.   : 0.0.1
 * Author : gynvael.coldwind//vx
 * Date   : 08.12.2003
 * Desc.  : like the sign says...
 */
#include<string.h>
#include"memory.h"

/* parse string helping stuff */
char*
jump_over_blank( char *where )
{
  char *now = where;

  /* jump over spaces and tabs until end of string */
  while( (*now) && ( (*now == ' ') || (*now == '\t') ) ) now++;

  return now;
}

char*
end_of_name( char *where )
{
  char *now = where;

  /* jump over to a space, a '=' or a tab */
  while( (*now) && ( (*now != ' ') &&
         (*now != '\t')  && (*now != '=') ) ) now++;

  return now;
}

char*
end_of_name_sp( char *where )
{
  char *now = where;

  /* jump over to a space */
  while( (*now) && ( (*now != ' ') ) ) now++;

  return now;
}

void
strip_eoln( char *where )
{
  char *now;

  /* locate eoln */
  now = strchr( where, '\n' );

  /* strip it */
  if( now )
  {
    *now = '\0';
  }
}

void
strip_end_blank( char *where )
{
  char *now = where + strlen( where ) - 1;

  /* jump over spaces and tabs until find sth else */
  while( (*now) && ( now >= where ) &&
       ( (*now == ' ') || (*now == '\t') ) ) now--;

  /* strip */
  if( (*now) && ( now >= where ) )
  {
    *(now+1) = '\0';
  }
}

char*
upper(char *what)
{
  int i;
  const char diff = 'a' - 'A';
  int size;

  /* is there a string? */
  if(!what) return NULL;

  /* is it empty? */
  size = strlen(what);
  if(!size) return NULL;

  /* upper */
  for(i=0; i<size; i++)
  {
    if((what[i]>='a') && (what[i]<='z'))
    {
      what[i]-=diff;
    }
  }

  return what;
}

const char*
copy_line( char *where, const char *what, int n )
{
  const char *now = what;
  int i = 0;

  /* copy till NULL \n or \r */
  while( ( i < n - 1 ) && *now && ( *now != '\n' ) && ( *now != '\r' ) )
  {
    *(where++) = *(now++);
    i++;
  }

  /* set '\0' */
  *where = '\0';

  /* move after \n and \r */
  if( *now=='\r' ) now++;
  if( *now=='\n' ) now++;

  return now;
}

