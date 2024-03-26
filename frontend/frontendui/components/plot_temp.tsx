"use client"
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

import { useEffect , useState} from 'react';

export default function MyResponsiveChart() {
  // Define the parameters for your function
  const m = 2; // slope
  const t = 3; // y-interceptcd ..

  const [useddata, setuseddata] = useState(0);
  const [stroke, setstroke] = useState("#8884d8");

  
  useEffect(() => {
    const interval = setInterval(() => {
      setuseddata(useddata => {
        if (useddata > 10) {
          console.log("Changing color");
          setstroke("green");
        }
        return useddata + 1;
      });
    }, 2000);
  
    // Clear interval on component unmount
    return () => {
      clearInterval(interval);
    };
  }, [useddata]); // Add useddata to the dependency array// Empty dependency array means this effect runs once on mount and clean up on unmount

  // Generate the data
  const data = Array.from({length: useddata}, (_, i) => ({name: `Point ${i}`, uv: m * i + t}));

  return (
    <ResponsiveContainer width="50%" height="100%">
      <LineChart width={500} height={300} data={data}>
        <XAxis dataKey="name" />
        <YAxis />
        <CartesianGrid strokeDasharray="3 3" />
        <Tooltip />
        <Legend />
        <Line type="monotone" dataKey="uv" stroke={stroke} activeDot={{ r: 8 }} />
      </LineChart>
    </ResponsiveContainer>
  );
}