import express, {Request, Response} from 'express'; 
import {getAllData} from "../services/DatabaseService"





export const GetAllEntries = async (request: Request, response: Response) => {
    console.log("ConstrollerHit")
    const dataResp = await getAllData()
    response.send(dataResp) 
  };
  