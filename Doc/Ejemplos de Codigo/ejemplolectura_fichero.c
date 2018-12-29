#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp;
	char str[2000];        /** Variable donde se guardara el texto del fichero*/
        int i;

	/* opening file for reading */
	fp = fopen("grafico.html", "r");
	if (fp == NULL) {
		perror("Error opening file");
		return -1;
	}
        /** se obtiene caracter a caracter el texto del fichero hasta encontrar el end of file*/
	while (!feof(fp)) {
                str[i] = fgetc(fp);
                i++;
	}
        printf ("%s", str);

	fclose(fp);

	return 0;
}
