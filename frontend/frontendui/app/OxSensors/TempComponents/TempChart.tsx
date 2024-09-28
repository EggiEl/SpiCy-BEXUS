import React, { useEffect, useRef, useState } from "react";
import TempSensorData, { TempDataClProps } from "../TempInterfaces/TempsensorData";
import CustomTooltipTemp from "./TempCustomTooltip"; 

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

const ChartContainer = styled.div`
  width: 100%;
  height: 400px;
`;

interface TempChartProps {
  datalist: TempSensorData[];
  limit: number;
  upperlimit: number;
  lowerlimit: number;
  measureTimeFeat: boolean;
  lowerLimitChange: number;
}

export default function TempChart({
  datalist,
  limit,
  upperlimit,
  lowerlimit,
  measureTimeFeat,
  lowerLimitChange,
}: TempChartProps) {
  const [timesBetween, setTimeBetween] = useState(0);
  const [timePoints, setTimePoints] = useState<string[]>([]);
  const [standardDot, setStandardDot] = useState({
    r: 2,
    fill: "transparent",
    onClick: (e: any, payload: any) => handleDotClick(payload.payload),
  });
  const [activeDot, setActiveDot] = useState({
    r: 20,
    fill: "red",
    onClick: (e: any, payload: any) => handleDotClick(payload.payload),
  });
  const timeFeatureRef = useRef(measureTimeFeat);
  const [firstTime, setFirstime] = useState("");
  const [secontTime, setSecondTime] = useState("");

  // Nur Daten im Bereich zwischen lowerLimitChange und limit anzeigen
  const dataToShow = datalist.slice(lowerLimitChange, limit);

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
  const handleDotClick = (data: TempSensorData) => {
    if (timeFeatureRef.current) {
      setTimePoints((prevTimePoints) => {
        if (prevTimePoints.length >= 0) {
          setFirstime(data.timestamp_measurement);
        }
        if (prevTimePoints.length >= 1) {
          const timeBetween =
            (new Date(data.timestamp_measurement).getTime() -
              new Date(prevTimePoints[prevTimePoints.length - 1]).getTime()) /
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
            {/* Linke Y-Achse für Temperatur */}
            <YAxis stroke="#333" />
            {/* Rechte Y-Achse für heaterPWM */}
            <YAxis
              yAxisId="right" // Eindeutige ID für die rechte Achse
              orientation="right"
              stroke="orange" // Farbe für heaterPWM
            />
            <Tooltip content={<CustomTooltipTemp />} />
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
            {/* Linie für Temperatur */}
            <Line
              type="monotone"
              dataKey="temperature"
              stroke={
                dataToShow.length > 0
                  ? dataToShow[dataToShow.length - 1].temperature > upperlimit
                    ? "red"
                    : dataToShow[dataToShow.length - 1].temperature < lowerlimit
                    ? "blue"
                    : "green"
                  : "green"
              }
              strokeWidth={2}
              dot={standardDot}
              activeDot={activeDot}
            />
            {/* Linie für heaterPWM, verbunden mit der rechten Y-Achse */}
            <Line
              type="monotone"
              dataKey="heaterPWM"
              stroke="orange"
              strokeWidth={2}
              dot={standardDot}
              activeDot={activeDot}
              yAxisId="right" // Verbindung mit der rechten Achse
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
