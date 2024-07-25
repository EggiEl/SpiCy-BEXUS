// app/OxSensors/page.tsx
import React from 'react';
import axios from 'axios';
import SensorPlotOx from './OxComponents/SensorPlotOx';
const fetchData = async () => {
  try {
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
    const data: OxygenSensorData[][] = await fetchData();
    const Oxsensor1 = data[0] ; 
    const Oxsensor2 = data[1] ;
    const Oxsensor3 = data[2] ;
    const Oxsensor4 = data[3] ;
    const Oxsensor5 = data[4] ;
    const Oxsensor6 = data[5] ;

  // Render a loading state if data is not yet available
  if (!data) {
    return <div>Error fetching Data reload the side and check the backend...</div>;
  }


 

  // Pass the fetched data to the client component
  return (
    <div >
        <SensorPlotOx initialData={Oxsensor1} /> 
  </div>
  )
};

export default OxSensorsPage;
