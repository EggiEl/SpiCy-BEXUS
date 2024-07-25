import express, {Request, Response, response} from 'express'; 
import {GetAllEntries, GetNewerEntries} from "../controllers/DatabaseController" 
import { GetAllOxygenSensorData } from '../controllers/DatabaseController';
import {GetNewerEntriesOxygen} from '../controllers/DatabaseController'; 

const databaserouter = express.Router() ; 

databaserouter.get('/storedData',  (request: Request, response) => {
    GetAllEntries(request, response)
    
}); 


databaserouter.get("/storedData_latest/:sensor/:id", (request: Request, response: Response) => 
{
    GetNewerEntries(request, response);
}); 


databaserouter.get("/oxygenSensor" , (request: Request, response: Response) =>
{
   const OxData =  GetAllOxygenSensorData(request, response);
});

databaserouter.get("/oxygenSensor_latest/:sensor/:id", (request: Request, response: Response) =>
{
    GetNewerEntriesOxygen(request, response);
} ) ; 

export default databaserouter
