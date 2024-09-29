



import axios from 'axios';
import { OtherDataProps } from '@/app/OxSensors/OtherPlots/OtherPlotInterface';

export async function get_latest_otherData(lastid : string): Promise<[OtherDataProps]> {
  try {
    const response = await axios.get(`http://localhost:8000/otherData/${lastid}`);
    
    return response.data;
  } catch (error: any) {
    if (error.response?.status === 404) {
      throw new Error('Data not found');
    }
    throw new Error('Failed to get data');
  }
}

