#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  if ( argc != 3 ) {
    printf("Usage: bdiff <file 1> <file 2>\n");
    return -1;
	}

  char const* const fn1 = argv[1];
  char const* const fn2 = argv[2];

  FILE* fp1 = fopen(fn1, "r");
	if ( !fp1 ) {
    printf("Error opening file \"%s\"\n", fn1);
    return -1;
	}

  FILE* fp2 = fopen(fn2, "r");
	if ( !fp2 ) {
    printf("Error opening file \"%s\"\n", fn2);
		fclose(fp1);
    return -1;
	}

  int c1, c2;
	int *buf1, *_buf1, *buf2, *_buf2;
  int maxsize = 32, len = 0, pos;
  int first_chunk = 1;

  while( 1 ) {
    c1 = fgetc(fp1);
    c2 = fgetc(fp2);

    if ( feof(fp1) || feof(fp2) )
      break;

    /* bytes differ so allocate buffers */
    if ( c1 != c2 ) {
      pos = ftell(fp1)-1;
      _buf1 = buf1 = malloc(maxsize * sizeof(*buf1));
      _buf2 = buf2 = malloc(maxsize * sizeof(*buf2));
    }

    /* we buffer everything until
     * a) the current diff chunk ends (when c1==c2) or
     * b) we reach the and of one of the two files or
     * c) we reach maxsize
     */
    while (c1 != c2 && !(feof(fp1) || feof(fp2)) ) {
      len++;
      *buf1++ = c1;
      *buf2++ = c2;

      if ( len == maxsize )
        break;

      c1 = fgetc(fp1);
      c2 = fgetc(fp2);
    }

    /* print the current diff chunk if there is one */
    if ( len > 0 ) {
      buf1 = _buf1;
      buf2 = _buf2;

      if ( first_chunk ) {
        printf("bdiff    : -%s+%s\n", fn1, fn2);
        first_chunk = 0;
      }

      printf("%08X : -", pos);
      /* left side */
      for (; (buf1-_buf1) < len; buf1++)
        printf("%02X", *buf1);
      /* right side */
      printf("+");
      for (; (buf2-_buf2) < len; buf2++)
        printf("%02X", *buf2);
      printf("\n");

      /* free everything for the next round */
      free(_buf1);
      free(_buf2);
      len = 0;
    }
  }

  if ( feof(fp1) ^ feof(fp2) ) {
    /* we reached the end of either fp1 or fp2, but not both.
     * This means the ...
     */

     printf("%08X : ", ftell(fp1));

    /* ... right side has more data */
    if ( feof(fp1)) {
       printf("+");
      do
        printf("%02X", c2);
      while ( (c2 = fgetc(fp2)) != EOF );
    }

     /* ... left side has more data */
    else if ( feof(fp2) ) {
      printf("-");
      do
        printf("%02X", c1);
      while ( (c1 = fgetc(fp1)) != EOF );
    }
    printf("\n");
  }

  fclose(fp1);
  fclose(fp2);

  return 0;
}
