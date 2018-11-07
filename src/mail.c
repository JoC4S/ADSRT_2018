#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail_lib.h"


int main(int argc, char const *argv[]) {

        sendmail("Este es el texto del mail de prueba desde la aplicación.\n Jose.");
        return 0;
}
