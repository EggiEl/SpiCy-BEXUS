import app from "./app";
import dotenv from "dotenv";
import { csvListener } from "./DataHandlers/csvListener";

//Get the Port from the env
dotenv.config();
const port = process.env.PORT ? parseInt(process.env.PORT) : 8000; 
console.log("Server starting...");
//Starte App

app.listen(port, 'localhost', () => {
  console.log(`Server listening at http://localhost:${port}`);
});