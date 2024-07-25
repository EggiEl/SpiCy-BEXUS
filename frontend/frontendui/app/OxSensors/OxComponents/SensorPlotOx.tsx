"use client";

import { useState, useEffect } from "react";
import Slider from "../../../components/sliderComp";
import OxChart from "./OxChart";
import  {get_latest_data} from "../../../functions/get_latest_ox";
interface OxygenSensorData {
  _id: string;
  fullstruct_id: string;
  percentOtwo: number;
  timestamp_measurement: string;
}

interface OxDataClProps {
  initialData: OxygenSensorData[];
  sensorname : string ; 
}

export default function SensorPlotOx({ initialData, sensorname  }: OxDataClProps) {
  const [data, setData] = useState(initialData);
  const [sliderLimit , setSliderLimit] = useState(data.length);
  const [limit, setLimit] = useState(sliderLimit); // Neuer Zustand für das Limit
  const increaseLimit = () => setLimit(prevLimit => prevLimit + 50); // Funktion zum Erhöhen des Limits
  const decreaseLimit = () => setLimit(prevLimit => Math.max(prevLimit - 50, 50)); // Funktion zum Verringern des Limits
  
  const fetchNewData = async () => {
    if (data.length > 0) {
      const lastData = data[data.length - 1];
      if (lastData && lastData._id) {
        const newData  : [OxygenSensorData] = await get_latest_data(lastData._id, sensorname);
        setData((prevData: any[]) => [...prevData, ...newData]);
        setSliderLimit(data.length);
        console.log(newData)
      }
    }
    if (data.length === 0) {
      const firstData = await get_latest_data("0", sensorname);
      console.log(firstData)
      setData(firstData)
    }
  };


 useEffect(() => {
    const intervalId = setInterval(fetchNewData, 1000); // Fetch new data every second
    return () => clearInterval(intervalId); // Clean up on component unmount
  }, [data]); // Add data as a dependency


  
  const handleLimitChange = (newLimit: number) => {
    setLimit(newLimit);
  };

  return (
    <div style={{backgroundColor :"white", color: "black"}}>
      <Slider upperlimit={data.length} lowerlimit={0} onLimitChange={handleLimitChange} />
      <div style={{ height: "500px" }}>
        <OxChart datalist={data} upperlimit={100} lowerlimit={40} limit={limit} />
      </div>
    </div>
  );
}
