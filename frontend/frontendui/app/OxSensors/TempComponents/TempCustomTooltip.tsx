import styled from "styled-components";


const CustomTooltipTemp= ({ active, payload }: any) => {
    if (active && payload && payload.length) {
      const data = payload[0].payload;
      return (
        <TooltipContainer>
          <p className="label">{`Time: ${data.timestamp_measurement}`}</p>
          <p className="intro">{`Temperatur Level: ${data.temperature}°C`}</p>
        </TooltipContainer>
      );
    }
  
    return null;
  };

  const TooltipContainer = styled.div`
  background-color: rgba(255, 255, 255, 0.9);
  border: 1px solid #ccc;
  padding: 10px;
  border-radius: 5px;
  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
`;

export default CustomTooltipTemp; 