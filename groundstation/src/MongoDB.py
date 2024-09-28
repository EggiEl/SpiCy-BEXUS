from datetime import datetime
from bson import Binary
from colored_terminal import *  # I added some colors to the consol prints. Changed only print(). Fynn
import struct
import pymongo 
import pymongo.errors

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

    def write_mongodb(self, struct: dict, db_name, collection_name):
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
            print(
                "Make sure you used the MongoDB.connect() method before writing to the database."
            )

    def get_raw_data(self, binary_data: bytes):
        """gets called by the DATA_LOGGER
        and recieves a C "struct packet" in form of bytes.\n
        These bytes are saved in a mongo database
        and then unpacked in a dictionary.
        This dictionary is then saved in a database as well.
        """
        """
        # struct packet
        # {                                     // struct_format L L 6L 6f 6f 6i i f 2i 80s
        #     unsigned int id = 0;              // each packet has a unique id
        #     unsigned int timestampPacket = 0; // in ms
        #     float power[2] = {0};             // battery voltage in mV and current consumption in mA
        #     struct OxygenReadout oxy_measure[6];
        #  --Struckt OxygenReadout sieht so aus:
        #       int32_t error = 0;
        #       int32_t dphi = 0;
        #       int32_t umolar = 0;
        #       int32_t mbar = 0;
        #       int32_t airSat = 0;
        #       int32_t tempSample = 0;
        #       int32_t tempCase = 0;
        #       int32_t signalIntensity = 0;
        #       int32_t ambientLight = 0;
        #       int32_t pressure = 0;
        #       int32_t humidity = 0;
        #       int32_t resistorTemp = 0;
        #       int32_t percentOtwo = 0;
        #       unsigned long timestamp_mesurement = 0;
        #  -- ende struct Oxygenreadout
        #     float light[12] = {0};
        #     /**temperature from thermistors:
        #      *0-5 NTC cable
        #      *6 NTC SMD
        #      *7 fix reference value
        #      *8 cpu temp*/
        #     float thermistor[9] = {0};
        #     float heaterPWM[6] = {0}; // power going to heating
        #     float pid[3] = {0}; //kp and ki
        # };
        """

        ## save raw data
        # TODO

        ## data conversion
        # creating the format string
        length_oxy_struct = 14  # items not bytes
        format_oxy = "14L "  # Format for one OxygenReadout struct
        format_packet = "2L 2f " + 6 * format_oxy + "12f 9f 6f 3f"
        
        #check sizes
        if struct.calcsize(format_packet) != len(binary_data):
            print_red(f"MongoDB: recived data size:{len(binary_data)} != struct size {struct.calcsize(format_packet)}")

        # Unpack all data
        unpacked_data = struct.unpack(format_packet, binary_data)

        # mapping the unpacked data to the respective fields
        decoded_data = {
            "ID": unpacked_data[0],
            "timestamp_packet": unpacked_data[1],
            "power": unpacked_data[2:4],
            "oxy_measure": [],
        }

        offset = 4
        for i in range(6):  # 6 oxygen readouts
            oxy = {
                # all devisions are valid data conversions in si-units. Look in the FD-OEM handbook for reference.
                "error": unpacked_data[offset],  # should stay 0 to indicate 0 errors
                "dphi": unpacked_data[offset + 1],  # wizard number
                "umolar": unpacked_data[offset + 2],  # wizard number
                "mbar": unpacked_data[offset + 3],  # not pressure!! some wizard number
                "airSat": unpacked_data[offset + 4],  # not in use as medium is liquid
                "tempSample": unpacked_data[offset + 5] / 100,  # °C
                "tempCase": unpacked_data[offset + 6] / 100,  # °C
                "signalIntensity": unpacked_data[offset + 7],  # mV
                "ambientLight": unpacked_data[offset + 8],  # mV
                "pressure": unpacked_data[offset + 9] / 100,  # mbar
                "humidity": unpacked_data[offset + 10] / 100,  # %
                # liquid medium -> should be 100%
                "resistorTemp": unpacked_data[offset + 11],  # should be 0. not in use
                "percentOtwo": unpacked_data[offset + 12],
                "timestamp_measurement": unpacked_data[offset + 13],  # in ms
            }
            decoded_data["oxy_measure"].append(oxy)
            offset += length_oxy_struct  # Move to the next OxygenReadout
            self.safeOx(oxy, "BEXUS", f"fullstruct_Sensor{i+1}", f"percentOtwo_Sensor{i+1}")

        decoded_data["thermistor"] = unpacked_data[offset : offset + 9]  # °C
        decoded_data["heaterPWM"] = unpacked_data[
            offset + 9 : offset + 9 + 6
        ]  # in % aka duty cycle
        decoded_data["pid"] = unpacked_data[
            offset + 9 + 6 : offset + 9 + 6 + 3
        ]  # W (values normed)
        print(decoded_data)
        ## save oxy
        # TODO
        ## save pressure
        # TODO



        #Get the Temperature of struct : 
        thermistor = decoded_data["thermistor"] 
        print("Thermistor: ", thermistor)
        for temp in range(len(thermistor)-3): 
            self.safe_temp({"temperature": thermistor[temp], "timestamp_measurement": datetime.now()}, "BEXUS", f"TemperatureSensor{temp+1}")

    def safe_temp(self, struct: dict, db_name, collection_name): 
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
            print(
                "Make sure you used the MongoDB.connect() method before writing to the database."
            )


    def safeOx(
        self, struct: dict, db_name, collection_name_fullstruct, collection_name_ox
    ):
        if self.client:
            try:
                mydb = self.client[db_name]
                mycol = mydb[collection_name_fullstruct]
                insert_result_full = mycol.insert_one(struct)
                insert_result_ox = mydb[collection_name_ox].insert_one(
                    {
                        "percentOtwo": struct["percentOtwo"],
                        "timestamp_measurement": struct["timestamp_measurement"],
                        "fullstruct_id": insert_result_full.inserted_id,
                    }
                )
                print("Eingefügte ID:", insert_result_full.inserted_id)
            except pymongo.errors.PyMongoError as e:
                print("Error inserting Data", e)
        else:
            print("No connection to database.")
            print(
                "Make sure you used the MongoDB.connect() method before writing to the database."
            )

    def safe_pressure_ox(self, struct: dict, db_name, collection_name_pressure):
        if self.client:
            try:
                mydb = self.client[db_name]
                mycol = mydb[collection_name_pressure]
                insert_pressure = mydb["pressure_ox"].insert_one(
                    {
                        "mbar": struct["mbar"],
                        "timestamp_measurement": struct["timestamp_measurement"],
                    }
                )
            except pymongo.errors.PyMongoError as e:
                print("Error inserting Data", e)
        else:
            print("No connection to database.")
            print(
                "Make sure you used the MongoDB.connect() method before writing to the database."
            )


# newData = { "Test" : 0 }
# mongodb = MongoDB("mongodb://localhost:27017")
# mongodb.connect()

# for i in range(10):
#     for j in range(6):
#             mongodb.safeOx({"error": 0,"dphi": 0, "umolar": 0, "mbar": 0, "airSat": 0, "tempSample": 0, "tempCase": 0, "signalIntensity": 0, "ambientLight": 0,"pressure": 0, "resistorTemp": 0,"percentOtwo": random.randint(0,100),"timestamp_measurement": datetime.now()}, "BEXUS", f"fullstruct_Sensor{1+ j}", f"percentOtwo_Sensor{1+ j}")

#     mongodb.safe_pressure_ox({"mbar": random.randint(0,100) + random.random(), "timestamp_measurement": datetime.now()}, "BEXUS", "pressure")
