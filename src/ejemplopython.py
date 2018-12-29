import sqlite3
import sys
import time

from sense_emu import SenseHat
#varieble para referencia del indice en base de datos
index = 0

def fill_line (angulo):

    if angulo > 0 and angulo <= 180:
            lado = "D"
            columna = 0
            nivel = angulo / 22.5
            inclinacion = angulo
    elif angulo > 180 and angulo < 360:
            lado = "I"
            columna = 4
            nivel = (360 - angulo ) / 22.5
            inclinacion = 360 - angulo
    else:
            lado = "C"
            nivel = 0
            columna = 2
            inclinacion = 0

    # Insert a row of data
    data = time.asctime( time.localtime(time.time()) )
    c.execute("insert into angulo values (?, ?, ?, ?)", (index, data, inclinacion, lado))
    # Save (commit) the changes
    conn.commit()

    print ("Index: %i |" % index), ("%s |" % data), ("Inclinacion = %2.2f |" % inclinacion), ("Lado: %c |" % lado)
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
    c.execute('CREATE TABLE angulo (Ind, Data , Inclinacion, Lado )')
except:
    print ('La base de datos ya contiene los campos')
    c.execute('SELECT MAX(Ind) FROM angulo')
    index = c.fetchone()[0]
# Se obtiene de la base de datos el mayor numero de indice registrado para asignarloa  "index"

print ('Valor de Ind: %d'% index), type(index)

sense = SenseHat()
green = 255

#bucle para la lectura constante de los valores del giroscopio
while True:
    o = sense.get_orientation()
    roll = o["roll"]
    sense.clear (0,0,0)
    fill_line(roll)
    time.sleep(1)
    index = index + 1

# We can also close the connection if we are done with it.
# Just be sure any changes have been committed or they will be lost.
conn.close()
