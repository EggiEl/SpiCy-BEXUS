import {Schema, model } from 'mongoose';

export interface IDataModel {
    temperature: number;
    timestamp_measurement: string;
    heaterPWM : number 
} 

const createDataModel = (sensorId: string) => {
    const DataSchema = new Schema<IDataModel>({
        temperature: { type: Number, required: true },
        timestamp_measurement: { type: String, required: true },
        heaterPWM : {type : Number , required : true}
    }, { collection: sensorId }); // Hier wird der Name der Collection festgelegt

    return model<IDataModel>(sensorId, DataSchema);
};

export const TemperatureSensor1 = createDataModel("TemperatureSensor1");
export const TemperatureSensor2 = createDataModel("TemperatureSensor2");
export const TemperatureSensor3 = createDataModel("TemperatureSensor3");
export const TemperatureSensor4 = createDataModel("TemperatureSensor4");
export const TemperatureSensor5 = createDataModel("TemperatureSensor5");
export const TemperatureSensor6 = createDataModel("TemperatureSensor6");

