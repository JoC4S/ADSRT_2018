#/***************************************************************************
#              Exemple uso SenseHat Raspberry Pi
#                             -------------------
#    begin                : Monday Dec 23 19:30:00 CET 2018
#    copyright            : (C) 2018 by
#    email                :
#***************************************************************************/
#/***************************************************************************
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#***************************************************************************/

import sqlite3
import sys
import time

from sense_emu import SenseHat
#varieble para referencia del indice en base de datos
index = 0

#fill_line se usa para llenar la base de datos con la informacion de angulo, temperatura y fecha

def fill_line (angulo):

    #Se discretiza en angulo en 8 niveles para poder mostrar en las 8 lineas del panel led del SenseHat.
    if angulo > 0 and angulo <= 180:
            lado = "D"
            columna = 0
            if angulo < 90:
                nivel = angulo / 12.85
            else:
                nivel = 7
            inclinacion = angulo
    elif angulo > 180 and angulo < 360:
            lado = "I"
            columna = 4
            if angulo >= 270:
                nivel =  (360 - angulo)  / 12.85
            else:
                nivel = 7
            inclinacion = angulo - 360
    else:
            lado = "C"
            nivel = 0
            columna = 2
            inclinacion = 0

    # Inserta una fila de informacion
    data = time.asctime( time.localtime(time.time()) )
    c.execute("insert into angulo values (?, ?, ?, ?, ?)", (index, data, inclinacion, lado, temp))
    # Guardar (commit) los cambios
    conn.commit()
    #imprimer por pantalla los datos guardados.
    print ("Index: %i |" % index), ("%s |" % data),("Angulo = %d |" % angulo), ("Inclinacion = %2.2f |" % inclinacion), ("Lado: %c |" % lado),("Temp: %2.2f C|" % temp)
    #Mostrar grado de inclinacion en el panel led del SenseHat
    nivel = int(nivel)
    i = 0
    while i < 4:
        x_pos = columna + i
        sense.set_pixel(x_pos, nivel, (0, green, 0))
        i += 1

#--------------------Main-------------------------
conn = sqlite3.connect('example.db')
print ('---> Abierta base de datos')

c = conn.cursor()

# Create table
try:
    c.execute('CREATE TABLE angulo (Ind, Data , Inclinacion, Lado, temperatura )')
except:
    print ('La base de datos ya contiene los campos')
    c.execute('SELECT MAX(Ind) FROM angulo')
    index = c.fetchone()[0]
# Se obtiene de la base de datos el mayor numero de indice registrado para asignarloa  "index"
#print ('Valor de Ind: %d' % index), type(index)

sense = SenseHat()
green = 255

#bucle para la lectura constante de los valores del giroscopio
while True:
    o = sense.get_orientation()
    temp = sense.get_temperature()
    roll = o["pitch"]
    sense.clear (0,0,0)
    fill_line(roll)
    #se toman muestras cada segundo
    time.sleep(0.5)
    index = index + 1

# Se cierra la conexion con la base de datos.
#todo aquello a lo que no se le haya hecho un commit, se perdera
conn.close()
