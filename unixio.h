/* Declarations for Unix style low level I/O functions */

#if __MSDOS__
	/* may be Turbo-C specific */
#	include <io.h>

/* DOS mkdir doesn't have a mode argument */
#define	mkdir(path,mode) mkdir(path)
#else

#	include <fcntl.h>
#	include <unistd.h>

#endif

/* sys/stat seems to be quite universal ... */

#include <sys/stat.h>

/* ... but some macros aren't */

#ifndef S_ISDIR
#	if defined (S_IFMT) && defined (S_IFDIR)
#		define	S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)
#	endif
#endif

#ifndef S_ISREG
#	if defined (S_IFMT) && defined (S_IFREG)
#		define	S_ISREG(mode)	(((mode) & S_IFMT) == S_IFREG)
#	endif
#endif

