import pymongo
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
                print("Error inserting Data", e)
        else:
            print("No connection to database.")
            print("Make sure you used the MongoDB.connect() method before writing to the database.")

    def safeOx(self, struct : dict, db_name, collection_name_fullstruct, collection_name_ox) : 
        if self.client:
            try: 
                mydb = self.client[db_name]
                mycol = mydb[collection_name_fullstruct]
                insert_result_full = mycol.insert_one(struct)
                insert_result_ox = mydb[collection_name_ox].insert_one({"percentOtwo": struct["percentOtwo"], "timestamp_measurement": struct["timestamp_measurement"], "fullstruct_id" : insert_result_full.inserted_id})

                print("Eingefügte ID:", insert_result_full.inserted_id)
            except pymongo.errors.PyMongoError as e:
                print("Error inserting Data", e)
        else:
            print("No connection to database.")
            print("Make sure you used the MongoDB.connect() method before writing to the database.")



newData = { "Test" : 0 }
mongodb = MongoDB("mongodb://localhost:27017")
mongodb.connect()

for i in range(10): 
    mongodb.safeOx({"error": 0,"dphi": 0, "umolar": 0, "mbar": 0, "airSat": 0, "tempSample": 0, "tempCase": 0, "signalIntensity": 0, "ambientLight": 0,"pressure": 0, "resistorTemp": 0,"percentOtwo": random.randint(0,100),"timestamp_measurement": datetime.now()}, "BEXUS", "fullstruct", "percentOtwo")

