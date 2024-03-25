import express, {Request, Response, response} from 'express'; 
import {GetAllEntries, GetNewerEntries} from "../controllers/DatabaseController" 



const databaserouter = express.Router() ; 

databaserouter.get('/storedData',  (request: Request, response) => {
    GetAllEntries(request, response)
    
}); 

export default databaserouter

databaserouter.get("/storedData_latest/:sensor/:id", (request: Request, response: Response) => 
{
    GetNewerEntries(request, response);
}); 