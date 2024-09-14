"use client";

import { useState, useEffect } from "react";
import OxChart from "./OxChart";
import { get_latest_data } from "../../../functions/get_latest_ox";
import styled from "styled-components";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData";
import { OxDataClProps } from "../OxInterfaces/ComponentProps";
import RangeSlider from "./newSlider";
import { Input } from "@nextui-org/react";

const Container = styled.div.attrs((props) => ({
  style: {
    backgroundColor: "white", // Dynamischer Hintergrund
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


export default function SensorPlotOx({ initialData, sensorname, measureTimeFeat }: OxDataClProps) {
  const [data, setData] = useState<OxygenSensorData[]>(initialData);
  const [range, setRange] = useState<number[]>([0, initialData.length]);
  const [sliderActive, setSliderActive] = useState(false);
  const [upperlimit, setUpperlimit] = useState(100);
  const [lowerlimit, setLowerlimit] = useState(0);
  const [dynamicplotlimit , setdynamicplotlimit] = useState(10);
  const fetchNewData = async () => {
    const lastData = data[data.length - 1];
    if (lastData && lastData._id) {
      const newData: [OxygenSensorData] = await get_latest_data(lastData._id, sensorname);
      console.log(newData);
      setData((prevData) => [...prevData, ...newData]);
    } else {
      const firstData = await get_latest_data("0", sensorname);
      console.log(firstData);
      setData(firstData);
    }
  };

  useEffect(() => {
    const intervalId = setInterval(fetchNewData, 1000);

    return () => clearInterval(intervalId);
  }, [data, sensorname]);

  useEffect(() => {
    if (!sliderActive) {
      const length = data.length;
      setRange([Math.max(length - dynamicplotlimit, 0), length]);
    }
  }, [data, sliderActive]);

  const handleRangeChange = (newRange: number[]) => {
    setRange(newRange);
  };

  const toggleSlider = () => {
    setSliderActive(!sliderActive);
    setRange([0, data.length]);
  };

  return (
    <Container color="black">
      <Header>{sensorname}</Header>
      <button onClick={toggleSlider}>Filter</button>

      {/* Input-Felder in einem flexiblen Layout */}
      <InputWrapper>
        <StyledInput
          key={"default"}
          type="number"
          color={"default"}
          label="Upper Limit"
          onChange={(e) => setUpperlimit(parseInt(e.target.value))}
        />
        <StyledInput
          key={"default"}
          type="number"
          color={"default"}
          label="Lower Limit"
          onChange={(e) => setLowerlimit(parseInt(e.target.value))}
        />
         <StyledInput
          key={"default"}
          type="number"
          color={"default"}
          label="Dynamic Plot Limit"
          onChange={(e) => setdynamicplotlimit(parseInt(e.target.value))}
        />
      </InputWrapper>

      {sliderActive && (
        <div style={{ width: "100%", display: "flex", justifyContent: "center" }}>
          <div style={{ width: "80%" }}>
            <h1>Einträge zwischen {range[0]} und {range[1]}</h1>
            <RangeSlider min={0} max={data.length} step={1} onChange={handleRangeChange} />
          </div>
        </div>
      )}
      <ChartWrapper>
        <OxChart
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
