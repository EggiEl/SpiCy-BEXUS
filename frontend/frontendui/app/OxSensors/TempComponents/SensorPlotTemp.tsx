"use client";

import { useState, useEffect } from "react";
import TempChart from "./TempChart";
import styled from "styled-components";
import RangeSlider from "../OxComponents/newSlider";
import TempSensorData, {TempDataClProps} from "../TempInterfaces/TempsensorData"
import { Input } from "@nextui-org/react";
// Neues Styled Component für das Input-Wrapper
const InputWrapper = styled.div`
  display: flex;
  justify-content: space-between;
  flex-wrap: wrap;
  gap: 20px;
  margin-bottom: 40px;
  background: #f0f0f0; 

  @media (max-width: 600px) {
    flex-direction: column;
  }
`;

const StyledInput = styled(Input)`
  width: 100%;
  max-width: 220px;
  padding: 10px;
  border-radius: 12px;
  background: linear-gradient(135deg, #f0f0f0, #e0e0e0);
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  transition: box-shadow 0.3s ease, transform 0.2s ease;
  border: 1px solid #ccc;

  &:hover {
    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15);
    transform: translateY(-2px);
  }

  &:focus-within {
    border-color: #0070f3;
    background: linear-gradient(135deg, #e0f7ff, #ccf0ff);
    box-shadow: 0 0 12px rgba(0, 112, 243, 0.4);
    transform: translateY(-4px);
  }

  /* Custom styling for the Input's label */
  .nextui-input-label {
    font-size: 0.9em;
    font-weight: bold;
    color: #666;
    transition: color 0.3s ease;
  }

  &:focus-within .nextui-input-label {
    color: #0070f3;
  }

  /* Style the placeholder for modern look */
  input::placeholder {
    color: #999;
    font-style: italic;
  }
`;

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
  font-size: 1em;
  margin-bottom: 5px;
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
  const [upperlimit, setUpperlimit] = useState(100);
  const [lowerlimit, setLowerlimit] = useState(0);
  const [dynamicplotlimit , setdynamicplotlimit] = useState(10);

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

      setRange([Math.max(length - dynamicplotlimit, 0), length]);

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
      <button style={{padding : 5, marginBottom: 10, fontSize : 10, borderRadius: 5}} onClick={toggleSlider}>
  &#128269; Filter
</button>  <InputWrapper>
        <Input
          key={"default1"}
          type="number"
          color={"default"}
          label="Upper Limit"
          onChange={(e) => setUpperlimit(parseInt(e.target.value))}
        />
        <Input
          key={"default2"}
          type="number"
          color={"default"}
          label="Lower Limit"
          onChange={(e) => setLowerlimit(parseInt(e.target.value))}
        />
         <Input
          key={"default3"}
          type="number"
          color={"default"}
          label="Dynamic Plot Limit"
          onChange={(e) => setdynamicplotlimit(parseInt(e.target.value))}
        />
      </InputWrapper>
      {sliderActive && (
        <div style={{ width: "100%", display: "flex", justifyContent: "center" }}>
          <div style={{ width: "80%" }}>
            <h1>Entries between {range[0]} and {range[1]}</h1>
            <RangeSlider min={0} max={data.length} step={1} onChange={handleRangeChange} />
          </div>
        </div>
      )}
      <ChartWrapper>
        <TempChart
          datalist={data}
          upperlimit={upperlimit}
          lowerlimit={lowerlimit}
          limit={range[1]}
          measureTimeFeat={measureTimeFeat}
          lowerLimitChange={range[0]}
        />
      </ChartWrapper>
    </Container>
  );
}
