import express, {Request, Response, response} from 'express'; 
import {GetAllEntries} from "../controllers/DatabaseController" 



const databaserouter = express.Router() ; 

databaserouter.get('/storedData',  (request: Request, response) => {
    GetAllEntries(request, response)
    
}); 

export default databaserouter
