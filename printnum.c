#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv)
{
long int number;

    if (argc != 2)
       return (1);

    number = atol (argv[1]);
    fwrite (&number, sizeof(long int), 1, stdout);

    return (0);
}

