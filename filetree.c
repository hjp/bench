#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>

#include "timer.h"
#include "unixio.h"
#include "pathmax.h"

#include "bench.h"
#include "filetree.h"

#define LEVELS 5
#define SUBDIRS 5
#define FILES 5
#define FILESIZE 1000

char buffer [FILESIZE];

int dirs = 0;
int files = 0;

static void createtree (const char *dirname, int levels) {
	int	i;
	FILE	*fp;

	if (mkdir (dirname, 0777) == -1) {
		int e = errno;
		fprintf (stderr, "createtree: cannot make directory %s: %s\n",
			 dirname, strerror (e));
		if (e != EEXIST) return;
	}
	dirs++;
	if (chdir (dirname) == -1) {
		fprintf (stderr, "createtree: cannot change to directory %s: %s\n",
			 dirname, strerror (errno));
		return;
	}
	if (levels > 0) {
		for (i = 0; i < SUBDIRS; i ++) {
			char newdir [10];
			sprintf (newdir, "d%d", i);
			createtree (newdir, levels - 1);
		}
	}
	for (i = 0; i < FILES; i ++) {
		char newfile [10];
		sprintf (newfile, "f%d", i);
		if ((fp = fopen (newfile, "wb")) == NULL) {
			fprintf (stderr, "createtree: cannot open file %s: %s\n",
				 newfile, strerror (errno));
			return;
		}
		files++;
		if (fwrite (buffer, 1, FILESIZE, fp) != FILESIZE) {
			fprintf (stderr, "createtree: cannot write to file %s: %s\n",
				 newfile, strerror (errno));
		}
		if (fclose (fp) == EOF) {
			fprintf (stderr, "createtree: cannot close file %s: %s\n",
				 newfile, strerror (errno));
		}
	}
	if (chdir ("..") == -1) {
		fprintf (stderr, "createtree: cannot change to directory %s: %s\n",
			 "..", strerror (errno));
		return;
	}
}


static void readtree (const char *dirname) {
	FILE		*fp;
	DIR		*dp;
	struct dirent	*de;

	if ((dp = opendir (dirname)) == NULL) {
		fprintf (stderr, "readtree: cannot open directory %s: %s\n",
			 dirname, strerror (errno));
		return;
	}

	if (chdir (dirname) == -1) {
		fprintf (stderr, "readtree: cannot change to directory %s: %s\n",
			 dirname, strerror (errno));
		return;
	}

	while ((de = readdir (dp))) {
		struct stat	sb;

		if (strcmp (de->d_name, ".") == 0 ||
		    strcmp (de->d_name, "..") == 0) continue;
		if (stat (de->d_name, &sb) == -1) {
			fprintf (stderr, "readtree: cannot stat %s: %s\n",
				 de->d_name, strerror (errno));
			continue;
		}
		if (S_ISDIR (sb.st_mode)) {
			readtree (de->d_name);
		} else if (S_ISREG (sb.st_mode)) {
			if ((fp = fopen (de->d_name, "rb")) == NULL) {
				fprintf (stderr, "readtree: cannot open file %s: %s\n",
					 de->d_name, strerror (errno));
				continue;
			}
			while (fread (buffer, 1, sizeof (buffer), fp) == sizeof (buffer));
			fclose (fp);
		}

	}
	closedir (dp);

	if (chdir ("..") == -1) {
		fprintf (stderr, "readtree: cannot change to directory %s: %s\n",
			 "..", strerror (errno));
		return;
	}
}


static void removetree (const char *dirname) {
	DIR		*dp;
	struct dirent	*de;

	if ((dp = opendir (dirname)) == NULL) {
		fprintf (stderr, "removetree: cannot open directory %s: %s\n",
			 dirname, strerror (errno));
		return;
	}

	if (chdir (dirname) == -1) {
		fprintf (stderr, "removetree: cannot change to directory %s: %s\n",
			 dirname, strerror (errno));
		return;
	}

	while ((de = readdir (dp))) {
		struct stat	sb;

		if (strcmp (de->d_name, ".") == 0 ||
		    strcmp (de->d_name, "..") == 0) continue;
		if (stat (de->d_name, &sb) == -1) {
			fprintf (stderr, "removetree: cannot stat %s: %s\n",
				 de->d_name, strerror (errno));
			continue;
		}
		if (S_ISDIR (sb.st_mode)) {
			removetree (de->d_name);
		} else {
			if (remove (de->d_name) != 0) {
				fprintf (stderr, "removetree: cannot remove %s: %s\n",
					 de->d_name, strerror (errno));
				continue;
			}
		}

	}
	closedir (dp);

	if (chdir ("..") == -1) {
		fprintf (stderr, "removetree: cannot change to directory %s: %s\n",
			 "..", strerror (errno));
		return;
	}
	if (rmdir (dirname) != 0) {
		fprintf (stderr, "removetree: cannot remove %s: %s\n",
			 dirname, strerror (errno));
		return;
	}
}


void filetree (char ***argvp) {
	double tr, tc;
	char cwd[PATH_MAX];

        (*argvp)++;

        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("File tree Bench:\n");
        else printf ("tree	| ");

        /* first get cwd so that we can return if anything goes wrong */
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
        	fprintf (stderr, "filetree: cannot get cwd: %s\n", strerror(errno));
        	return;
	}

        resettimer ();

	dirs = files = 0;
	createtree ("ft", LEVELS);

        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\ntree (%d levels, %d directories, %d files) created in %g seconds\n",
			LEVELS, dirs, files,
			tr);
		printf ("CPU time: %g seconds\n", tc);
	} else {
		printf ("%d	| %d	| %d	| %g	| %g	| ",
			LEVELS, dirs, files,
			tr, tc);
	}

        if (chdir(cwd) == -1) {
        	fprintf (stderr, "filetree: cannot change to %s: %s\n", cwd, strerror(errno));
        	return;
        }

	resettimer ();
	readtree ("ft");
        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\ntree read in %g seconds\n", tr);
		printf ("CPU time: %g seconds\n", tc);
	} else {
		printf("%g	| %g	| ", tr, tc);
	}

        if (chdir(cwd) == -1) {
        	fprintf (stderr, "filetree: cannot change to %s: %s\n", cwd, strerror(errno));
        	return;
        }

        resettimer ();
	removetree ("ft");
        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\ntree removed in %g seconds\n", tr);
		printf ("CPU time: %g seconds\n", tc);
	} else {
		printf("%g	| %g	| ", tr, tc);
	}
	printf("\n");

}
