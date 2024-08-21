"use client";

import { useState, useEffect } from "react";
import OxChart from "./OxChart";
import { get_latest_data } from "../../../functions/get_latest_ox";
import styled from "styled-components";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData";
import { OxDataClProps } from "../OxInterfaces/ComponentProps";
import RangeSlider from "./newSlider";

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

export default function SensorPlotOx({ initialData, sensorname, measureTimeFeat }: OxDataClProps) {
  const [data, setData] = useState<OxygenSensorData[]>(initialData);
  const [range, setRange] = useState<number[]>([0, initialData.length]);
  const [sliderActive, setSliderActive] = useState(false);

  const fetchNewData = async () => {
    const lastData = data[data.length - 1];
    if (lastData && lastData._id) {
      const newData: [OxygenSensorData] = await get_latest_data(lastData._id, sensorname);
      console.log(newData)
      setData(prevData => [...prevData, ...newData]);
    }
    else {
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
      setRange([Math.max(length - 10, 0), length]);
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
