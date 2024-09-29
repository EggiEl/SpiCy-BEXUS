
import { otherDataChartProps } from "./InterfaceOtherData";


export interface OtherDataProps{
    _id: string;
    datalist: Array<otherDataChartProps>;
    limit: number;
    upperlimit: number;
    lowerlimit: number;
    measureTimeFeat: boolean; 
    lowerLimitChange: number;
    datakeyValue: string ;
    

}