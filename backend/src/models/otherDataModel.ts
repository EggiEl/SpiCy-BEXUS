import {Schema, model } from 'mongoose';

export interface IDataModel {
    pressure : number ; 
    TemperaturMotherboard: number;
    TemperaturCPU: number;
    BatteryVoltage: number;
    BatteryCurrent: number;
    timestamp_measurement: string;
} 



/* 
TemperaturMotherboard
106
TemperaturCPU
108
BatteryVoltage
2
BatteryCurrent
3
timestamp_measurement
2024-09-29T09:14:13.863+00:00
*/ 


const createDataModel = (sensorId: string) => {
    const DataSchema = new Schema<IDataModel>({
        TemperaturMotherboard  : { type: Number, required: true }, 
        TemperaturCPU : { type: Number, required: true },
        BatteryVoltage : { type: Number, required: true },
        BatteryCurrent : { type: Number, required: true },
        timestamp_measurement: { type: String, required: true },
        pressure: { type: Number, required: true },
    }, { collection: sensorId }); // Hier wird der Name der Collection festgelegt

    return model<IDataModel>(sensorId, DataSchema);
};




export const otherData = createDataModel("otherData")
