// components/Slider.js
import { useState, useEffect } from 'react';

interface SliderProps { 
  upperlimit: number, 
  lowerlimit: number,
  onLimitChange: (value: number) => void
}

const Slider = ({ upperlimit, onLimitChange }: SliderProps) => {
  const [value, setValue] = useState(upperlimit);

  useEffect(() => {
    onLimitChange(value);
  }, [value, onLimitChange]);

  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setValue(Number(event.target.value));
  };

  return (
    <div style={{ width: '300px', margin: '0 auto', textAlign: 'center' }}>
      <input
        type="range"
        min={1}
        max={upperlimit}
        value={value}
        onChange={handleChange}
        style={{ width: '100%' }}
      />
      <p>Value: {value}</p>
    </div>
  );
};

export default Slider;
