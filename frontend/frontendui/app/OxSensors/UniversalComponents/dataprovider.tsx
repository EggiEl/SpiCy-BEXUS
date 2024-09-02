"use client";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData";
import PlotField from "../OxComponents/plotfield";
import Navbar from "../OxComponents/navbar";
import { useState } from "react";

interface DataproviderProps {
    oxygenInitialData: OxygenSensorData[][];
}

export default function Dataprovider({ oxygenInitialData }: DataproviderProps) {
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
            {!oxPlot && <div>Temp Plot</div>}
        </div>
    );
}
