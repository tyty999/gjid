#include <stdio.h>
#include <vga.h>
#include <vgagl.h>

int main (int argc, char ** argv)
{
FILE * fp;
char buffer[768];

    if (argc != 2) {
       printf ("Usage: %s <file.pal>\n", argv[0]);
       return (0);
    }

    vga_init();
    vga_setmode (G320x200x256);
    gl_setcontextvga (G320x200x256);
    gl_getpalettecolors (0, 255, buffer);
    vga_setmode (TEXT);

    if ((fp = fopen (argv[1], "wb")) != NULL) {
       fwrite (buffer, 768, 1, fp);
       fclose (fp);
    }
    else
       printf ("Could not open %s!\n", argv[1]);

    return (0);
}
