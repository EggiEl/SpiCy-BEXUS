"use client";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData";
import PlotField from "../OxComponents/plotfield";
import TempPlotfield from "../TempComponents/TempPlotfield"
import Navbar from "../OxComponents/navbar";
import { useState } from "react";
import TempSensorData from "../TempInterfaces/TempsensorData"
import ZoomableChart from "../testplot";
import OtherPlotField from "../OtherPlots/OtherPlotField";
import { OtherDataProps } from "../OtherPlots/OtherPlotInterface";

interface DataproviderProps {
    oxygenInitialData: OxygenSensorData[][];
    tempInitialData: TempSensorData[][]; 
    otherPlotInitialData : OtherDataProps[]

}

export default function Dataprovider({ oxygenInitialData, tempInitialData, otherPlotInitialData }: DataproviderProps) {
    const [oxPlot, setOxPlot] = useState(1);


    


    return (
        <div >
           
            <Navbar  setActivePlot={setOxPlot} activePlot={oxPlot} />
            <div style={{paddingTop : 40}}> 
            {oxPlot===1 && 
            <div > 
            <PlotField plotInitalData={oxygenInitialData} />
          
            </div>
            }
            {oxPlot===2 && <TempPlotfield plotInitalData={tempInitialData} />}
            </div>

            <div style={{color:"white"}}> 
            {oxPlot === 3  && 
            <OtherPlotField plotInitalData={otherPlotInitialData}/> 
            }
            </div>

           
            

        </div>
    );
}
