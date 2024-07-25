import { Schema, model } from 'mongoose';

export interface IDataModel {
 
    percentOtwo: number;
    timestamp_measurement: Date;
    fullstruct_id: string;
}

const createOxModel = (sensorId: string) => {
    const DataSchema = new Schema<IDataModel>({
        percentOtwo: { type: Number, required: true },
        timestamp_measurement: { type: Date, required: true },
        fullstruct_id: { type: String, required: true }
    }, { collection: sensorId }); // Hier wird der Name der Collection festgelegt

    return model<IDataModel>(sensorId, DataSchema);
};

export const Oxmodel1 = createOxModel("percentOtwo_Sensor1");
export const Oxmodel2 = createOxModel("percentOtwo_Sensor2");
export const Oxmodel3 = createOxModel("percentOtwo_Sensor3");
export const Oxmodel4 = createOxModel("percentOtwo_Sensor4");
export const Oxmodel5 = createOxModel("percentOtwo_Sensor5");
export const Oxmodel6 = createOxModel("percentOtwo_Sensor6");

