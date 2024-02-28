import pymongo

class MongoDB: 
    def __init__(self, uri, db_name, db_collection_name) -> None:
          self.uri = uri
          self.db_name = db_name
          self.db_collection_name = db_collection_name
          self.client = None 

    def connect(self): 
        try:
            # Verbindung zur MongoDB-Datenbank herstellen
            self.client = pymongo.MongoClient(self.uri)
            print("Verbunden mit der Datenbank")
        except pymongo.errors.PyMongoError as e:
            print("Fehler beim Verbinden mit der DB:", e)

    def write_mongodb(self, struct: dict) : 
        if self.client:
            try: 
                mydb = self.client[self.db_name]
                mycol = mydb[self.db_collection_name]
                insert_result = mycol.insert_one(struct)
                print("Eingefügte ID:", insert_result.inserted_id)
            except pymongo.errors.PyMongoError as e:
                print("Fehler beim Schreiben in die DB:", e)
        else:
            print("Keine Verbindung zur Datenbank vorhanden.")



