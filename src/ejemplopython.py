import sqlite3
import sys

from sense_emu import SenseHat

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
    # This is the qmark style:
    dato1 = "dia-hora"
    c.execute("insert into angulo values (?, ?, ?)", (dato1, inclinacion, lado))
    # Save (commit) the changes
    conn.commit()

    nivel = int (nivel)
    print ("Inclinacion = %f" % inclinacion)

    i = 0
    while i < 4:
        x_pos = columna + i
        sense.set_pixel(x_pos, nivel, (0, green, 0))
        i += 1



conn = sqlite3.connect('example.db')

c = conn.cursor()

# Create table
c.execute('CREATE TABLE angulo (Data , Inclinacion, Lado )')

sense = SenseHat()
green = 255

while True:
    o = sense.get_orientation()
    roll = o["roll"]
    sense.clear (0,0,0)
    fill_line(roll)

# We can also close the connection if we are done with it.
# Just be sure any changes have been committed or they will be lost.
conn.close()
