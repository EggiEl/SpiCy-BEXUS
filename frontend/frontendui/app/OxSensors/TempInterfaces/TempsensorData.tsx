export default interface TempSensorData { 
    _id: string;
    temperature: number;
    timestamp_measurement: string;
    heatPower : string 
} 



export interface TempDataClProps {
    initialData: TempSensorData[];
    sensorname: string;
    measureTimeFeat : boolean ; 
  }