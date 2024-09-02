"use client";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData";
import PlotField from "../OxComponents/plotfield";
import TempPlotfield from "../TempComponents/TempPlotfield"
import Navbar from "../OxComponents/navbar";
import { useState } from "react";
import TempSensorData from "../TempInterfaces/TempsensorData"
interface DataproviderProps {
    oxygenInitialData: OxygenSensorData[][];
    tempInitialData: TempSensorData[][]; 
}

export default function Dataprovider({ oxygenInitialData, tempInitialData }: DataproviderProps) {
    const [oxPlot, setOxPlot] = useState(true);

    const showOxPlot = () => {
        setOxPlot(true);
    };

    const showTempPlot = () => {
        setOxPlot(false);
    };

    return (
        <div>
           
            <Navbar showOxPlot={showOxPlot} showTempPlot={showTempPlot} activePlot={oxPlot ? 'ox' : 'temp'} />
       
            {oxPlot && <PlotField plotInitalData={oxygenInitialData} />}
            {!oxPlot && <TempPlotfield plotInitalData={tempInitialData} />}
            

        </div>
    );
}
