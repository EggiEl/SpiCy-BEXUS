"use client"

import { useState, useEffect } from 'react';
import MyResponsiveChart from '../../components/plot_temp';
import {get_latest_data} from '../../functions/get_latest_data';
import {get_all_data} from '../../functions/get_all_data';






export default function LandingPage() {
  const [data, setData] = useState<any[]>([]);
  const [limit, setLimit] = useState(100); // Neuer Zustand für das Limit

  const fetchNewData = async () => {
    if (data.length > 0) {
      const lastData = data[data.length - 1];
      if (lastData && lastData._id) {
        const newData = await get_latest_data(lastData._id, "Sensor1");
        console.log(newData);
        setData((prevData: any[]) => [...prevData, ...newData]);
      }
    }
  }

  const fetchAllData = async () => {
    const allData = await get_all_data();
    console.log(allData);
    setData(allData[0]);
  }

  const increaseLimit = () => setLimit(prevLimit => prevLimit + 50); // Funktion zum Erhöhen des Limits
  const decreaseLimit = () => setLimit(prevLimit => Math.max(prevLimit - 50, 50)); // Funktion zum Verringern des Limits
  const zoomout = () => setLimit(data.length)


  useEffect(() => {
    fetchAllData();
  }, []);

  useEffect(() => {
    const intervalId = setInterval(fetchNewData, 1000); // Fetch new data every second
    return () => clearInterval(intervalId); // Clean up on component unmount
  }, [data]); // Add data as a dependency

  return(
    <div>
      <h1>
        Hallo Welt
      </h1>
      <button onClick={fetchNewData}>
        Daten abrufen
      </button>
      <button onClick={zoomout}> Zoom out</button>
      <button onClick={fetchAllData}>
        Alle Daten
      </button>
      <button onClick={increaseLimit}>
        Limit erhöhen
      </button>
      <button onClick={decreaseLimit}>
        Limit verringern
      </button>
      <div style={{ height: '500px' }}>

      <MyResponsiveChart datalist={data} limit={limit}/>
      </div>
      <div style={{ height: '500px' }}>

      <MyResponsiveChart datalist={data} limit={limit}/>
      </div>

    </div>
  )
}