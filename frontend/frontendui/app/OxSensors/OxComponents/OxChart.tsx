import React from "react";
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from "recharts";

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
      <div className="custom-tooltip" style={{backgroundColor : "white", color : "black", padding : 20}}>
        <p className="label">{`Time: ${data.timestamp_measurement}`}</p>
        <p className="intro">{`Oxygen Level: ${data.percentOtwo}`}</p>
      </div>
    );
  }

  return null;
};

export default function OxChart({ datalist, limit, upperlimit, lowerlimit }: Data) {
  const dataToShow = datalist.slice(Math.max(datalist.length - limit, 0)); // Show only the last 'limit' data points
  console.log(dataToShow);

  return (
    <ResponsiveContainer width="100%" height="100%">
      <LineChart data={dataToShow}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="timestamp_measurement" />
        <YAxis />
        <Tooltip content={<CustomTooltip />} />
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
        />
      </LineChart>
    </ResponsiveContainer>
  );
}
