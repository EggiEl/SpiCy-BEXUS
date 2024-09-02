// app/OxSensors/page.tsx
import React from 'react';
import axios from 'axios';
import Dataprovider from './UniversalComponents/dataprovider';

const fetchData = async () => {
  try {
    console.log("Inital Fetch")
    const response = await axios.get('http://localhost:8000/oxygenSensor');
    return response.data;
  } catch (error) {
    console.error('Error fetching data:', error);
    return null;
  }
};

interface OxygenSensorData {
    _id: string;
    fullstruct_id: string;
    percentOtwo: number;
    timestamp_measurement: string;
  }
  



const OxSensorsPage = async () => {
    const oxygensensordata: OxygenSensorData[][] = await fetchData();
    

  // Render a loading state if data is not yet available
  if (!oxygensensordata) {
    return <div>Error fetching Data reload the side and check the backend...</div>;
  }


 

  // Pass the fetched data to the client component
  return (
  <div >
    <Dataprovider oxygenInitialData={oxygensensordata} /> 
    
  </div>
  )
};

export default OxSensorsPage;
