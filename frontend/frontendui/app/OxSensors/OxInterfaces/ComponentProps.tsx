
import { OxygenSensorData } from "./OxygenSensorData"; 

export interface OxChartProps {
    datalist: Array<OxygenSensorData>;
    limit: number;
    upperlimit: number;
    lowerlimit: number;
    measureTimeFeat: boolean; 
    lowerLimitChange: number;
  }


export interface OxDataClProps {
    initialData: OxygenSensorData[];
    sensorname: string;
    measureTimeFeat : boolean ; 
  }