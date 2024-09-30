import React, { useEffect, useRef, useState } from "react";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData";
import { OxChartProps } from "../OxInterfaces/ComponentProps";
import CustomTooltip from "./TooltipOther";


import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  ReferenceLine,
} from "recharts";
import styled from "styled-components";
import { OtherDataProps } from "./OtherPlotInterface";

const ChartContainer = styled.div`
  width: 100%;
  height: 400px;
`;

export default function OtherChart({
  datalist,
  limit,
  upperlimit,
  lowerlimit,
  measureTimeFeat,
  lowerLimitChange,
  datakeyValue,
}: OtherDataProps) {
  const [timesBetween, setTimeBetween] = useState(0);
  const [timePoints, setTimePoints] = useState<string[]>([]);
  const [standardDot, setStandardDot] = useState({
    r: 2,
    fill: "transparent",
    onClick: (e: any, payload: any) => handleDotClick(payload.payload),
  });
  const [activeDot, setActiveDot] = useState({
    r: 8, // Hier kleinerer Radius für Hoverpunkt
    fill: "red",
    onClick: (e: any, payload: any) => handleDotClick(payload.payload),
  });
  const timeFeatureRef = useRef(measureTimeFeat);
  const [firstTime, setFirstime] = useState("");
  const [secontTime, setSecondTime] = useState("");

  // Nur Daten im Bereich zwischen lowerLimitChange und limit anzeigen
  const dataToShow = datalist.slice(lowerLimitChange, limit);

  // Berechnung des minimalen und maximalen Werts für die Y-Achse

  // Update the timeFeatureRef when the measureTimeFeat changes and update the dot size and color
  useEffect(() => {
    timeFeatureRef.current = measureTimeFeat;

    if (measureTimeFeat) {
      setStandardDot({
        r: 8,
        fill: "blue",
        onClick: (e: any, payload: any) => handleDotClick(payload.payload),
      });
    } else {
      setFirstime("");
      setSecondTime("");
      setStandardDot({
        r: 0,
        fill: "transparent",
        onClick: (e: any, payload: any) => handleDotClick(payload.payload),
      });
    }
  }, [measureTimeFeat]);

  // Function to handle the click on a dot in the chart
  const handleDotClick = (data: OxygenSensorData) => {
    if (timeFeatureRef.current) {
      setTimePoints((prevTimePoints) => {
        if (prevTimePoints.length >= 0) {
          setFirstime(data.timestamp_measurement);
        }
        if (prevTimePoints.length >= 1) {
          const timeBetween =
            (new Date(data.timestamp_measurement).getTime() -
              new Date(
                prevTimePoints[prevTimePoints.length - 1]
              ).getTime()) /
            1000;

          setSecondTime(prevTimePoints[prevTimePoints.length - 1]);
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
            <YAxis 
              stroke="#333" 
             // Achse auf min und max Werte der Daten einstellen
            />
            <Tooltip content={<CustomTooltip/>} />
            <ReferenceLine
              y={upperlimit}
              label="Upper Limit"
              stroke="red"
              strokeDasharray="3 3"
            />
            <ReferenceLine
              y={lowerlimit}
              label="Lower Limit"
              stroke="blue"
              strokeDasharray="3 3"
            />
            <Line
              type="monotone"
              dataKey={datakeyValue}
              stroke={
                "orange"
              }
              strokeWidth={2}
              dot={standardDot}
              activeDot={activeDot}
            />
          </LineChart>
        </ResponsiveContainer>
      </ChartContainer>
      {firstTime && <p>{firstTime}</p>}
      {secontTime && <p>{secontTime}</p>}
      <p>Time between last two clicks: {timesBetween} seconds</p>
    </div>
  );
}
