
import {Sensor1, Sensor2, Sensor3,Sensor4, Sensor5, Sensor6}  from "../models/datamodel"
import {Models, Schema, model } from 'mongoose';
import mongoose, { Model } from 'mongoose';


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
