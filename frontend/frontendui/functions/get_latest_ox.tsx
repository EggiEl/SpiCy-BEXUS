



import axios from 'axios';


interface SensorData { 
  _id: string;
  percentOtwo: number;
  timestamp_measurement: string;
  fullstruct_id: string;
}

export async function get_latest_data(lastid : string , sensorname : string): Promise<[SensorData]> {
  try {
    const response = await axios.get(`http://localhost:8000/oxygenSensor_latest/${sensorname}/${lastid}`);
    
    return response.data;
  } catch (error: any) {
    if (error.response?.status === 404) {
      throw new Error('Data not found');
    }
    throw new Error('Failed to get data');
  }
}

