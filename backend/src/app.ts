import express, { Express } from "express";
import bodyParser from "body-parser"; 
import databaserouter from "./routes/DatabaseRouter";
import cors from "cors"; 
import mongoose from 'mongoose';





const app : Express = express() 


app.use(cors()); 
app.use(bodyParser.json()) ;
app.use(databaserouter) ; 


const uri = "mongodb://localhost:27017/elouvredb"

export async function connect() : Promise<void> {
    try {
      await mongoose.connect(uri); 
      console.log("Connceted to MongoDB")
    } 
    
    catch (error) {
      console.error(error)
    }
  }

export default app ; 
