import express, {Request, Response} from 'express'; 
import {getAllData, getNewerEntries} from "../services/DatabaseService"
import { getAllOxygenSensorDataService, getNewerEntriesOxygen } from '../services/DatabaseService'; 


export const GetAllEntries = async (request: Request, response: Response) => {
    console.log("ConstrollerHitAllEntries")
    const dataResp = await getAllData()
    response.send(dataResp) 
  };
  
export const GetNewerEntries = async (request: Request, response: Response) => {
    console.log("ConstrollerHitNewerEntries")
    const sensor = request.params.sensor;
    const id = request.params.id;
    
    const dataResp = await getNewerEntries(sensor, id);
    

    response.send(dataResp)
  }

export const GetAllOxygenSensorData = async (request: Request, response: Response) => {
    console.log("ConstrollerHitOxygenSensorData"); 
    const OxData  = await getAllOxygenSensorDataService() ; 
    response.send(OxData)
  }


export const GetNewerEntriesOxygen = async (request: Request, response: Response) => {
    console.log("ConstrollerHitNewerEntriesOxygen")
    const id = request.params.id;
    const sensor = request.params.sensor;
    const newestDataResp = await getNewerEntriesOxygen(sensor, id);
    response.send(newestDataResp)
   console.log(newestDataResp)
  }