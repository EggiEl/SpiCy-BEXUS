"use client";

import { useState, useEffect } from "react";
import Slider from "../../../components/sliderComp";
import OxChart from "./OxChart";
import { get_latest_data } from "../../../functions/get_latest_ox";
import styled from "styled-components";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData"
import { OxDataClProps } from "../OxInterfaces/ComponentProps"; 


const Container = styled.div`
  background-color: white;
  color: black;
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

export default function SensorPlotOx({ initialData, sensorname , measureTimeFeat}: OxDataClProps) {
  const [data, setData] = useState(initialData);
  const [sliderLimit, setSliderLimit] = useState(data.length);
  const [limit, setLimit] = useState(sliderLimit); // State for the limit

  const increaseLimit = () => setLimit(prevLimit => prevLimit + 50); // Function to increase the limit
  const decreaseLimit = () => setLimit(prevLimit => Math.max(prevLimit - 50, 50)); // Function to decrease the limit

  const fetchNewData = async () => {
    if (data.length > 0) {
      const lastData = data[data.length - 1];
      if (lastData && lastData._id) {
        const newData: [OxygenSensorData] = await get_latest_data(lastData._id, sensorname);
        setData(prevData => [...prevData, ...newData]);
        setSliderLimit(data.length);
        console.log(newData);
      }
    } else {
      const firstData = await get_latest_data("0", sensorname);
      console.log(firstData);
      setData(firstData);
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
    <Container>
      <Header>{sensorname}</Header>
      <Slider upperlimit={data.length} lowerlimit={0} onLimitChange={handleLimitChange} />
      <ChartWrapper>
        <OxChart datalist={data} upperlimit={100} lowerlimit={40} limit={limit} measureTimeFeat = {measureTimeFeat}/>
      </ChartWrapper>
    </Container>
  );
}
