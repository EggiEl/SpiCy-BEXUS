"use client"

import { useState, useEffect } from 'react';
import MyResponsiveChart from '../components/plot_temp';
import {get_latest_data} from '../functions/get_latest_data';
import {get_all_data} from '../functions/get_all_data';

export default function LandingPage() {
  const [data, setData] = useState([]);
  const [limit, setLimit] = useState(20); // Neuer Zustand für das Limit

  const fetchData = async () => {
    const data = await get_latest_data("6601d39a57c1ff9b7787bd03", "Sensor1");
    console.log(data);
  }

  const fetchAllData = async () => {
    const allData = await get_all_data();
    console.log(allData);
    setData(allData[0] );
  }

  const increaseLimit = () => setLimit(prevLimit => prevLimit + 1); // Funktion zum Erhöhen des Limits
  const decreaseLimit = () => setLimit(prevLimit => Math.max(prevLimit - 1, 1)); // Funktion zum Verringern des Limits

  useEffect(() => {
    fetchAllData();
  }, []);

  return(
    <div>
      <h1>
        Hallo Welt
      </h1>
      <button onClick={fetchData}>
        Daten abrufen
      </button>
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
    </div>
  )
}