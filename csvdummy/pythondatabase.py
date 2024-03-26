import pymongo
import time
import random
from datetime import datetime

class MongoDB: 
    def __init__(self, uri) -> None:
          self.uri = uri
          self.client = None 

    def connect(self): 
        try:
            # Verbindung zur MongoDB-Datenbank herstellen
            self.client = pymongo.MongoClient(self.uri)
            print("Verbunden mit der Datenbank")
        except pymongo.errors.PyMongoError as e:
            print("Fehler beim Verbinden mit der DB:", e)

    def write_mongodb(self, struct: dict, db_name, collection_name ) : 
        if self.client:
            try: 
                mydb = self.client[db_name]
                mycol = mydb[collection_name]
                insert_result = mycol.insert_one(struct)
                print("Eingefügte ID:", insert_result.inserted_id)
            except pymongo.errors.PyMongoError as e:
                print("Fehler beim Schreiben in die DB:", e)
        else:
            print("Keine Verbindung zur Datenbank vorhanden.")


mongodb = MongoDB("localhost:27017")
mongodb.connect()
eintrag = 1 
for i in range(100): 
    mongodb.write_mongodb({"temperature" : random.randint(10,20), "time" : datetime.now(), "eintragsnummer" : eintrag}, "testDB","Sensor1" )
    eintrag+=1 