import React from 'react';
import { Range, getTrackBackground } from 'react-range';
import styled from 'styled-components';

interface RangeSliderProps {
  min: number;
  max: number;
  step: number;
  onChange: (values: number[]) => void;
}

const SliderWrapper = styled.div`
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 20px;
  width: 100%;
`;

const Track = styled.div<{ background: string }>`
  height: 8px;
  width: 100%;
  border-radius: 4px;
  background: ${({ background }) => background};
  position: relative;
`;

const Thumb = styled.div`
  height: 24px;
  width: 24px;
  background-color: #007bff;
  border-radius: 50%;
  box-shadow: 0px 2px 6px rgba(0, 0, 0, 0.2);
  display: flex;
  justify-content: center;
  align-items: center;
  cursor: pointer;
`;

const ThumbLabel = styled.div`
  position: absolute;
  top: -35px;
  background-color: #007bff;
  color: white;
  font-size: 12px;
  padding: 4px 8px;
  border-radius: 4px;
`;

const RangeSlider: React.FC<RangeSliderProps> = ({ min, max, step, onChange }) => {
  const [values, setValues] = React.useState<number[]>([min, max]);

  const handleChange = (newValues: number[]) => {
    setValues(newValues);
    onChange(newValues);
  };

  return (
    <SliderWrapper>
      <Range
        step={step}
        min={min}
        max={max}
        values={values}
        onChange={handleChange}
        renderTrack={({ props, children }) => (
          <Track
            {...props}
            background={getTrackBackground({
              values,
              colors: ['#ccc', '#007bff', '#ccc'],
              min,
              max,
            })}
          >
            {children}
          </Track>
        )}
        renderThumb={({ props, value, isDragged }) => (
          <Thumb {...props}>
            {isDragged && (
              <ThumbLabel>{value}</ThumbLabel>
            )}
          </Thumb>
        )}
      />
    </SliderWrapper>
  );
};

export default RangeSlider;
