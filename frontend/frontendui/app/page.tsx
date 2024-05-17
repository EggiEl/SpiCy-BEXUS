import { get_all_data } from '@/functions/get_all_data';
import {get_latest_data} from "@/functions/get_latest_data"
import FullPlot from "@/components/FullPlotComponent" ; 

export const dynamic = 'force-dynamic';

const LandingPage = async () => {
  let mountingdata = await get_all_data();
  console.log(mountingdata)
  let dataSensor1 = mountingdata[0] ; 
  let dataSensor2 = mountingdata[1] ;  
  const dataSensor3  = mountingdata[2] ; 
  const dataSensor4 = mountingdata[3] 
  console.log(dataSensor1);


  console.log(mountingdata);
  
  
  return (
    <div>
      <h1>Mountingdata</h1>
      <FullPlot  plotdata={dataSensor1} plotlimit={100} sensorname='Sensor1' upperlimit={25} lowerlimit={24} />
      <FullPlot  plotdata={dataSensor2} plotlimit={100} sensorname='Sensor2' upperlimit={25} lowerlimit={24} />

    </div>
  );
}

export default LandingPage;
