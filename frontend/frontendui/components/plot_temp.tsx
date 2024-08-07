import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';

interface Data {
  
  datalist : Array<{temperature: number, time: string, eintragsnummer: number, _id: string}>,
  limit: number, // Neue Eigenschaft hinzufügen
  upperlimit : number , 
  lowerlimit : number 
}

export default function MyResponsiveChart({datalist, limit, upperlimit, lowerlimit}: Data) {
  const dataToShow = datalist.slice(Math.max(datalist.length - limit, 0)); // Nur die letzten 'limit' Datenpunkte anzeigen

  return (
    <ResponsiveContainer width="100%" height="100%">
      <LineChart data={dataToShow}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="time" />
        <YAxis />
        <Tooltip />
        <Line 
  type="monotone" 
  dataKey="temperature" 
  stroke={
    dataToShow.length > 0 
      ? dataToShow[dataToShow.length - 1].temperature > upperlimit 
        ? 'red' 
        : dataToShow[dataToShow.length - 1].temperature < lowerlimit 
          ? 'blue'
          : 'green'
      : 'green'
  } 
/>      </LineChart>
    </ResponsiveContainer>
  );
}