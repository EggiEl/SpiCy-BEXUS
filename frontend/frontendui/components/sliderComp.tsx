// components/Slider.js
import { useState, useEffect } from 'react';

interface SliderProps { 
  upperlimit: number, 
  lowerlimit: number,
  onLimitChange: (value: number, lowerlimitVal: number) => void
}

const Slider = ({lowerlimit, upperlimit, onLimitChange }: SliderProps) => {
  const [value, setValue] = useState(upperlimit);
  const [lowerlimitVal, setLowerlimit] = useState(lowerlimit);

  useEffect(() => {
    onLimitChange(value, lowerlimitVal);
  }, [value, onLimitChange, lowerlimitVal]);

  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setValue(Number(event.target.value));
  };

  const handleLowerLimitChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setLowerlimit(Number(event.target.value));
  };

  return (
    <div style={{ width: '300px', margin: '0 auto', textAlign: 'center' }}>
      <input
        type="range"
        min={lowerlimitVal}
        max={upperlimit}
        value={value}
        onChange={handleChange}
        style={{ width: '100%' }}
      />
      <input
        type="range"
        min={0}
        max={value}
        value={lowerlimitVal}
        onChange={handleLowerLimitChange}
        style={{ width: '100%' }}
      />

      <p>Value: {value}</p>
      <p>Lower Limit: {lowerlimitVal}</p> 
    </div>
  );
};

export default Slider;
