#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmail.h"


int main(int argc, char const *argv[]) {

        char mailto[] = "RCPT TO: <jose.cayero@me.com>\n";
        char Contenido[1000] = "Texto del mail.\nFin.\n";

        sendmail(mailto, Contenido);
        return 0;
}
