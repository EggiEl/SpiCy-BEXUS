import axios from 'axios';

export async function get_all_data(): Promise<any[]> {
  try {
    const response = await axios.get(`http://localhost:8000/storedData`);
    
    return response.data;
  } catch (error: any) {
    if (error.response?.status === 404) {
      throw new Error('Data not found');
    }
    throw new Error('Failed to get data');
  }
}