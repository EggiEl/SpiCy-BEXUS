"use client";

import { useState, useEffect } from 'react';
import MyResponsiveChart from '../components/plot_temp';
import { get_latest_data } from '../functions/get_latest_data';

export interface ComponentProps { 
    plotdata: any[], 
    plotlimit: number, 
    sensorname: string, 
    lastdataID: any , 
    upperlimit : number , 
    lowerlimit : number 
}

export default function FullPlot({ plotdata, plotlimit, sensorname, upperlimit, lowerlimit }: ComponentProps) {
  const [data, setData] = useState(plotdata);
  const [limit, setLimit] = useState(plotlimit); // Neuer Zustand für das Limit

  const fetchNewData = async () => {
    if (data.length > 0) {
      const lastData = data[data.length - 1];
      if (lastData && lastData._id) {
        const newData = await get_latest_data(lastData._id, sensorname);
        console.log(newData);
        setData((prevData: any[]) => [...prevData, ...newData]);
      }
  
    }
    if( data.length === 0 ) {
      const firstData = await get_latest_data("0", sensorname);
      console.log(firstData)
      setData(firstData)
    }
  }

  


  const increaseLimit = () => setLimit(prevLimit => prevLimit + 50); // Funktion zum Erhöhen des Limits
  const decreaseLimit = () => setLimit(prevLimit => Math.max(prevLimit - 50, 50)); // Funktion zum Verringern des Limits
  const zoomout = () => setLimit(data.length)

  useEffect(() => {
    const intervalId = setInterval(fetchNewData, 1000); // Fetch new data every second
    return () => clearInterval(intervalId); // Clean up on component unmount
  }, [data]); // Add data as a dependency

  return (
    <div>
      <h1>Hallo Welt</h1>
      <button onClick={fetchNewData}>Daten abrufen</button>
      <button onClick={zoomout}>Zoom out</button>
      <button onClick={increaseLimit}>Limit erhöhen</button>
      <button onClick={decreaseLimit}>Limit verringern</button>
      {data.length !== 0 ? (
        <div style={{ height: '500px' }}>
          <MyResponsiveChart datalist={data} limit={limit} upperlimit={upperlimit} lowerlimit={lowerlimit}/>
        </div>
      ) : (
        <h1> Suche nach Daten für Sensor {sensorname} </h1>
      )}
    </div>
  );
}
