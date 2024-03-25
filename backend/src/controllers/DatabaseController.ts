import express, {Request, Response} from 'express'; 
import {getAllData, getNewerEntries} from "../services/DatabaseService"




export const GetAllEntries = async (request: Request, response: Response) => {
    console.log("ConstrollerHit")
    const dataResp = await getAllData()
    response.send(dataResp) 
  };
  
export const GetNewerEntries = async (request: Request, response: Response) => {
    console.log("ConstrollerHit")
    const sensor = request.params.sensor;
    const id = request.params.id;
    const dataResp = await getNewerEntries(sensor, id);
    response.send(dataResp);
  }