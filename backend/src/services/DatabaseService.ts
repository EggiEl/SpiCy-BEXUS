
import {Sensor1, Sensor2, Sensor3,Sensor4, Sensor5, Sensor6}  from "../models/datamodel"
import mongoose from 'mongoose';




export const getAllData = async() => {
    console.log("getAllData")
    try {
        const newdata = new Sensor1({temperature : 200, time:  "200"})
        await newdata.save()
        const Sensor1Data = await Sensor1.find().lean()
        const Sensor2Data = await Sensor2.find().lean()
        const Sensor3Data = await Sensor3.find().lean()
        const Sensor4Data = await Sensor4.find().lean()
        const Sensor5Data = await Sensor5.find().lean()
        const Sensor6Data = await Sensor6.find().lean()
        const allData = [Sensor1Data, Sensor2Data, Sensor3Data, Sensor4Data, Sensor5Data, Sensor6Data]
        return allData
    } 
    catch (error) {
        console.error(error)
    }
    
}