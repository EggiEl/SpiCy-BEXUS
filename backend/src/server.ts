import app from "./app";
import dotenv from "dotenv";
import { csvListener } from "./DataHandlers/csvListener";

//Get the Port from the env
dotenv.config();
const port = process.env.PORT;






console.log("Server starting...");
//Starte App

app.listen(port , () => {
  console.log(`Server started at http://localhost:${port}`); 

})