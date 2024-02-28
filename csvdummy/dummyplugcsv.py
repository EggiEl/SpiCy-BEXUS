import csv
from random import randint
import time
from pythondatabase import MongoDB 

names = ["Elias", "Fynn", "Amanda", "Eva"]
powerlevels = [2, 3, 4]
battlescream = ["Heia", "jihaaa", "beatemup"]
my_db = MongoDB("mongodb://localhost:27017/", "Test", "Testcollect")

# Verbinde mit der Datenbank
my_db.connect()


while True:
    with open('data.csv', mode='a', newline='') as file:  # Öffne die CSV-Datei im Anhänge-Modus
        writer = csv.writer(file)
        structer = [names[randint(0, 3)], powerlevels[randint(0, 2)], battlescream[randint(0, 2)]]
        writer.writerow(structer)  # Schreibe die aktuelle Zeile in die CSV-Datei
        print(structer)
        # Schreibe Daten in die Datenbank
        my_db.write_mongodb({"name": names[randint(0, 3)], "battlescream" : battlescream[randint(0, 2)], "Powerlvl":  powerlevels[randint(0, 2)]})
    time.sleep(2)
