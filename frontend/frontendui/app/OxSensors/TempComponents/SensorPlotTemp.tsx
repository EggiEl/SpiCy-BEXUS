"use client";

import { useState, useEffect } from "react";
import TempChart from "./TempChart";
import styled from "styled-components";
import RangeSlider from "../OxComponents/newSlider";
import TempSensorData, {TempDataClProps} from "../TempInterfaces/TempsensorData"

const Container = styled.div.attrs((props) => ({
  style: {
    backgroundColor:  "white", // Dynamischer Hintergrund
    color: props.color || "black", // Dynamische Farbe
  },
}))`
  padding: 20px;
  border-radius: 10px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
  max-width: 100%;
  margin: 20px auto;
`;

const Header = styled.h1`
  text-align: center;
  font-size: 2em;
  margin-bottom: 80px;
  color: #333;
`;

const ChartWrapper = styled.div`
  height: 500px;
  margin-top: 20px;
`;


import axios from 'axios';


export async function get_latest_data(lastid : string , sensorname : string): Promise<[TempSensorData]> {
  try {
    const response = await axios.get(`http://localhost:8000/storedData_latest/${sensorname}/${lastid}`);
    
    return response.data;
  } catch (error: any) {
    if (error.response?.status === 404) {
      throw new Error('Data not found');
    }
    throw new Error('Failed to get data');
  }
}

// This component is the parent of the sonsor plot and the slider 
// It fetches new data from the backend and passes it to the sensor plot component
// It also handles the slider and the range of the data shown in the plot
// The data is fetched every second
export default function SensorPlotTemp({ initialData, sensorname, measureTimeFeat }: TempDataClProps) {
  const [data, setData] = useState<TempSensorData[]>(initialData);
  const [range, setRange] = useState<number[]>([0, initialData.length]);
  const [sliderActive, setSliderActive] = useState(false);

// Function to fetch new data from the backend
  const fetchNewData = async () => {
    const lastData = data[data.length - 1];
    if (lastData && lastData._id) {
      const newData: [TempSensorData] = await get_latest_data(lastData._id, sensorname);
      console.log(newData)
      setData(prevData => [...prevData, ...newData]);
    }
    else {
      const firstData = await get_latest_data("0", sensorname);
      console.log(firstData);
      setData(firstData);
    }
  };


  //Function to fetch new data every second
  useEffect(() => {
    const intervalId = setInterval(fetchNewData, 1000);
    
    return () => clearInterval(intervalId);
  }, [data, sensorname]);


  //Function to set the range of the slider to the last 10 entries
  useEffect(() => {
    if (!sliderActive) {
      const length = data.length;
      const plotLimit = 10 ; 
      setRange([Math.max(length - plotLimit, 0), length]);

    }
    
  }, [data, sliderActive]);

  
//function to handle the change of the range of the slider
  const handleRangeChange = (newRange: number[]) => {
    setRange(newRange);
  };
  //function to handle to toggle the slider
  const toggleSlider = () => {
    setSliderActive(!sliderActive);
    setRange([0, data.length]);
    
  };

  return (
    <Container color="black">      
    <Header>{sensorname}</Header>
      <button onClick={toggleSlider}>Filter</button>
      {sliderActive && (
        <div style={{ width: "100%", display: "flex", justifyContent: "center" }}>
          <div style={{ width: "80%" }}>
            <h1>Einträge zwischen {range[0]} und {range[1]}</h1>
            <RangeSlider min={0} max={data.length} step={1} onChange={handleRangeChange} />
          </div>
        </div>
      )}
      <ChartWrapper>
        <TempChart
          datalist={data}
          upperlimit={100}
          lowerlimit={40}
          limit={range[1]}
          measureTimeFeat={measureTimeFeat}
          lowerLimitChange={range[0]}
        />
      </ChartWrapper>
    </Container>
  );
}
