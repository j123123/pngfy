#include "basename.h"
#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR '/'
#endif

#if defined (_WIN32) || defined (__MSDOS__) || defined (__DJGPP__) || defined (__OS2__)
#define HAVE_DOS_BASED_FILE_SYSTEM
#ifndef DIR_SEPARATOR_2 
#define DIR_SEPARATOR_2 '\\'
#endif
#endif

/* Define IS_DIR_SEPARATOR.  */
#ifndef DIR_SEPARATOR_2
# define IS_DIR_SEPARATOR(ch) ((ch) == DIR_SEPARATOR)
#else /* DIR_SEPARATOR_2 */
# define IS_DIR_SEPARATOR(ch) \
    (((ch) == DIR_SEPARATOR) || ((ch) == DIR_SEPARATOR_2))
#endif /* DIR_SEPARATOR_2 */

char *basename (const char *name)
{
  const char *base;
#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
    /* Skip over the disk name in MSDOS pathnames. */
    if (isalpha(name[0]) && name[1] == ':') 
        name += 2;
#endif
    for (base = name; *name; name++)
    {
        if (IS_DIR_SEPARATOR (*name))
        {
            base = name + 1;
        }
    }
    return (char *) base;
}
