import React from "react";
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

interface OxygenSensorData {
  _id: string;
  fullstruct_id: string;
  percentOtwo: number;
  timestamp_measurement: string;
}

interface Data {
  datalist: Array<OxygenSensorData>;
  limit: number;
  upperlimit: number;
  lowerlimit: number;
}

const CustomTooltip = ({ active, payload }: any) => {
  if (active && payload && payload.length) {
    const data = payload[0].payload;
    return (
      <TooltipContainer>
        <p className="label">{`Time: ${data.timestamp_measurement}`}</p>
        <p className="intro">{`Oxygen Level: ${data.percentOtwo}%`}</p>
      </TooltipContainer>
    );
  }

  return null;
};

const TooltipContainer = styled.div`
  background-color: rgba(255, 255, 255, 0.9);
  border: 1px solid #ccc;
  padding: 10px;
  border-radius: 5px;
  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
`;

const ChartContainer = styled.div`
  width: 100%;
  height: 400px;
`;

export default function OxChart({ datalist, limit, upperlimit, lowerlimit }: Data) {
  const dataToShow = datalist.slice(Math.max(datalist.length - limit, 0)); // Show only the last 'limit' data points

  return (
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
            dot={{ stroke: 'black', strokeWidth: 1, r: 1 }}
            activeDot={{ r: 4 }}
          />
        </LineChart>
      </ResponsiveContainer>
    </ChartContainer>
  );
}
