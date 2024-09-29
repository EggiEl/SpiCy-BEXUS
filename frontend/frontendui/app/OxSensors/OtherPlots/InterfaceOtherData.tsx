import { OtherDataProps } from "./OtherPlotInterface";


export interface otherDataChartProps {
    datalist: Array<OtherDataProps>;
    limit: number;
    upperlimit: number;
    lowerlimit: number;
    measureTimeFeat: boolean; 
    lowerLimitChange: number;
    plottedValue: string;
  }


export interface otherDataClProps {
    otherDataList: OtherDataProps[];
    sensorname: string;
    measureTimeFeat : boolean ; 
    datakeyValue: string ; 
  }