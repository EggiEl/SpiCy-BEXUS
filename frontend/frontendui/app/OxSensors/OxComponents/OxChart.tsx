import React, { useEffect, useRef } from "react";
import { useState } from "react";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData"
import CustomTooltip from "./CustomTooltip"; 
import { OxChartProps } from "../OxInterfaces/ComponentProps"; 
import {LineChart,Line,XAxis,YAxis,CartesianGrid,Tooltip,ResponsiveContainer, ReferenceLine} from "recharts";
import styled from "styled-components";




const ChartContainer = styled.div`
  width: 100%;
  height: 400px;
`;

export default function OxChart({ datalist, limit, upperlimit, lowerlimit , measureTimeFeat}: OxChartProps) {
  const [timesBetween, setTimeBetween] = useState(0);
  const [timePoints, setTimePoints] = useState<string[]>([]);
  const dataToShow = datalist.slice(Math.max(datalist.length - limit, 0)); // Show only the last 'limit' data points
  const [standardDot, setStandardDot] = useState({ r: 2, fill: 'transparent', onClick: (e: any, payload: any) => handleDotClick(payload.payload) });
  const [activeDot, setActiveDot] = useState({ r: 20, fill: 'red', onClick: (e: any, payload: any) => handleDotClick(payload.payload) });
  const timeFeatureRef = useRef(measureTimeFeat);
  const [firstTime ,setFirstime] = useState("")
  const [secontTime , setSecondTime] = useState("")

  useEffect(() => {
    timeFeatureRef.current = measureTimeFeat;

    if (measureTimeFeat) {
      setStandardDot({ r: 8, fill: 'blue', onClick: (e: any, payload: any) => handleDotClick(payload.payload) });

    } else {
      setFirstime("")
      setSecondTime("")
      setStandardDot({ r: 2, fill: 'transparent', onClick: (e: any, payload: any) => handleDotClick(payload.payload) });

    }
  }, [measureTimeFeat]);

  const handleDotClick = (data: OxygenSensorData) => {
    if (timeFeatureRef.current ) {
      setTimePoints((prevTimePoints) => {
        if (prevTimePoints.length >= 0 ) { 
          setFirstime(data.timestamp_measurement)
        }
        if (prevTimePoints.length >= 1  ) {
          const timeBetween = (new Date(data.timestamp_measurement).getTime() - new Date(prevTimePoints[prevTimePoints.length - 1]).getTime()) / 1000;
          
          setSecondTime(prevTimePoints[prevTimePoints.length - 1])
          setTimeBetween(timeBetween);
          return [data.timestamp_measurement];
        } else {

          return [...prevTimePoints, data.timestamp_measurement];
          
        }
      });
    } else {
      
      console.log("Feature not active");
    }
  };

  return (
    <div>
      <ChartContainer>
        <ResponsiveContainer width="100%" height="100%">
          <LineChart data={dataToShow}>
            <CartesianGrid strokeDasharray="3 3" stroke="#e0e0e0" />
            <XAxis dataKey="timestamp_measurement" stroke="#333" />
            <YAxis stroke="#333" />
            <Tooltip content={<CustomTooltip />} />
            <ReferenceLine y={upperlimit} label="Upper Limit" stroke="red" strokeDasharray="3 3" />
            <ReferenceLine y={lowerlimit} label="Lower Limit" stroke="blue" strokeDasharray="3 3" />
            <Line
              type="monotone"
              dataKey="percentOtwo"
              stroke={
                dataToShow.length > 0
                  ? dataToShow[dataToShow.length - 1].percentOtwo > upperlimit
                    ? "red"
                    : dataToShow[dataToShow.length - 1].percentOtwo < lowerlimit
                    ? "blue"
                    : "green"
                  : "green"
              }
              strokeWidth={2}
              dot={standardDot}
              activeDot={activeDot}
            />
          </LineChart>
        </ResponsiveContainer>
      </ChartContainer>
      {firstTime && (
        <p>{firstTime}</p>
      ) }
      {secontTime && (
        <p>{secontTime}</p>
      ) }
      <p>Time between last two clicks: {timesBetween} seconds</p>
    </div>
  );
}
