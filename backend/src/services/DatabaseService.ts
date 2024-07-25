
import {Sensor1, Sensor2, Sensor3,Sensor4, Sensor5, Sensor6}  from "../models/datamodel"
import {Models, Schema, model } from 'mongoose';
import mongoose, { Model } from 'mongoose';
import { Oxmodel1 , Oxmodel2 , Oxmodel3, Oxmodel4, Oxmodel5, Oxmodel6} from "../models/Oxmodel";

export type SensorType = "Sensor1" | "Sensor2" | "Sensor3" | "Sensor4" | "Sensor5" | "Sensor6";


export const getAllData = async() => {
    console.log("getAllData")
    try {
        const Sensor1Data = await Sensor1.find().lean()
        const Sensor2Data = await Sensor2.find().lean()
        const Sensor3Data = await Sensor3.find().lean()
        const Sensor4Data = await Sensor4.find().lean()
        const Sensor5Data = await Sensor5.find().lean()
        const Sensor6Data = await Sensor6.find().lean()
        const allData = [Sensor1Data, Sensor2Data, Sensor3Data, Sensor4Data, Sensor5Data, Sensor6Data]
        console.log(allData)
        return allData
    } 
    catch (error) {
        console.error(error)
    }
    
}



export const  getNewerEntries = async(sensor: string, id: string) => {
    
    try {
        let sensorModel;
        switch (sensor) {
            case "Sensor1":
                sensorModel = Sensor1;
                break;
            case "Sensor2":
                sensorModel = Sensor2;
                break;
            case "Sensor3":
                sensorModel = Sensor3;
                break;
            case "Sensor4":
                sensorModel = Sensor4;
                break;
            case "Sensor5":
                sensorModel = Sensor5;
                break;
            case "Sensor6":
                sensorModel = Sensor6;
                break;

            default:
                throw new Error("Invalid sensor type");
                
        }

        if (id === "0") { 
            const first = await sensorModel.find().lean()
            console.log("first entry")
            return first;

        }
        else { 
            const newerEntries = await sensorModel.find({ _id: { $gt: id } }).lean().exec();
            console.log("newer entry")
            return newerEntries;

        }
    } catch (error) {
        console.error('Error retrieving newer entries:', error);
        return [] 
        
    }
}


export const getAllOxygenSensorDataService = async() => {
    console.log("getAllOxygenSensorData")
    try {
        console.log("OxSensor")
        const OxSensor1 = await Oxmodel1.find().lean()
        const OxSensor2 = await Oxmodel2.find().lean()
        const OxSensor3 = await Oxmodel3.find().lean()
        const OxSensor4 = await Oxmodel4.find().lean()
        const OxSensor5 = await Oxmodel5.find().lean()
        const OxSensor6 = await Oxmodel6.find().lean()
        const OxData = [OxSensor1, OxSensor2, OxSensor3, OxSensor4, OxSensor5, OxSensor6]
        console.log(OxData); 
        return OxData; 
    } 
    catch (error) {
        console.error(error)
    }
    
}


export const  getNewerEntriesOxygen = async(sensor: string, id: string) => {
    
    try {
        let sensorModel;
        switch (sensor) {
            case "Sensor1":
                sensorModel = Oxmodel1;
                break;
            case "Sensor2":
                sensorModel = Oxmodel2;
                break;
            case "Sensor3":
                sensorModel = Oxmodel3;
                break;
            case "Sensor4":
                sensorModel = Oxmodel4;
                break;
            case "Sensor5":
                sensorModel = Oxmodel5;
                break;
            case "Sensor6":
                sensorModel = Oxmodel6;
                break;

            default:
                throw new Error("Invalid sensor type");
                
        }

        if (id === "0") { 
            const first = await sensorModel.find().lean()
            console.log("first entry")
            return first;

        }
        else { 
            console.log(sensorModel)
            const newerEntries = await sensorModel.find({ _id: { $gt: id } }).lean().exec();
            console.log("newer entry")
            return newerEntries;

        }
    } catch (error) {
        console.error('Error retrieving newer entries:', error);
        return [] 
        
    }
}