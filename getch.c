#include <stdio.h>
#include "getch.h"

int getch ()
{
	char	s [80];

	fgets (s, 80, stdin);
	return s [0];
}
