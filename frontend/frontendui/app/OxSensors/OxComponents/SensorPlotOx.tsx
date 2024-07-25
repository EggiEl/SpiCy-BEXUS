"use client";

import { useState, useEffect } from "react";
import Slider from "../../../components/sliderComp";
import OxChart from "./OxChart";

interface OxygenSensorData {
  _id: string;
  fullstruct_id: string;
  percentOtwo: number;
  timestamp_measurement: string;
}

interface OxDataClProps {
  initialData: OxygenSensorData[];
}

export default function SensorPlotOx({ initialData }: OxDataClProps) {
  console.log(initialData);
  const [sliderLimit , setSliderLimit] = useState(initialData.length);
  const [limit, setLimit] = useState(sliderLimit); // Neuer Zustand für das Limit
  
  const increaseLimit = () => setLimit(prevLimit => prevLimit + 50); // Funktion zum Erhöhen des Limits
  const decreaseLimit = () => setLimit(prevLimit => Math.max(prevLimit - 50, 50)); // Funktion zum Verringern des Limits
  


  
  const handleLimitChange = (newLimit: number) => {
    setLimit(newLimit);
  };

  return (
    <div style={{backgroundColor :"white", color: "black"}}>
      <Slider upperlimit={sliderLimit} lowerlimit={0} onLimitChange={handleLimitChange} />
      <div style={{ height: "500px" }}>
        <OxChart datalist={initialData} upperlimit={100} lowerlimit={40} limit={limit} />
      </div>
    </div>
  );
}
