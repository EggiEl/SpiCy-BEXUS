import express, {Request, Response} from 'express'; 
import {getAllData, getNewerEntries} from "../services/DatabaseService"




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
    if (dataResp.length > 0) {
    response.send(dataResp);
    }
    else response.send([])
  }