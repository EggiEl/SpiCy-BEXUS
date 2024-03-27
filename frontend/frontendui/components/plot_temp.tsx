"use client"
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

import { useEffect , useState} from 'react';

export default function MyResponsiveChart({ m = 2, t = 3 }) {
  const [useddata, setUsedData] = useState(0);
  const [stroke, setStroke] = useState("#8884d8");

  useEffect(() => {
    const interval = setInterval(() => {
      setUsedData(usedData => {
        if (usedData > 10) {
          console.log("Changing color");
          setStroke("green");
        }
        return usedData + 1;
      });
    }, 2000);
  
    return () => {
      clearInterval(interval);
    };
  }, [useddata]);

  const data = Array.from({ length: useddata }, (_, i) => ({ name: `Point ${i}`, uv: m * i + t }));

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
