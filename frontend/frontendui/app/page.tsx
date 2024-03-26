"use client"

import MyResponsiveChart from '../components/plot_temp';
import {get_latest_data} from '../functions/get_latest_data';


  export default function LandingPage() {

    const fetchData = async () => {
      const data = await get_latest_data("6601d39a57c1ff9b7787bd03", "Sensor1");
      console.log(data);
    }




    return(
      <div>
          <h1>
            Hallo Welt
          </h1>
          <button onClick={fetchData}>
        Daten abrufen
      </button>
          <div style={{ height: '500px' }}>
          <MyResponsiveChart />
        </div>
      </div>
    )
  } 

