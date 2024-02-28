import express, { Express } from "express";
import bodyParser from "body-parser"





const app : Express = express() 

app.use(bodyParser.json()) ;




export default app ; 
