import {Schema, model } from 'mongoose';

export interface IDataModel {
    temperature: number;
    time: string;
} 

const createDataModel = (sensorId: string) => {
    const DataSchema = new Schema<IDataModel>({
        temperature: { type: Number, required: true },
        time: { type: String, required: true }
    }, { collection: sensorId }); // Hier wird der Name der Collection festgelegt

    return model<IDataModel>(sensorId, DataSchema);
};

export const Sensor1 = createDataModel("Sensor1");
export const Sensor2 = createDataModel("Sensor2");
export const Sensor3 = createDataModel("Sensor3");
export const Sensor4 = createDataModel("Sensor4");
export const Sensor5 = createDataModel("Sensor5");
export const Sensor6 = createDataModel("Sensor6");

