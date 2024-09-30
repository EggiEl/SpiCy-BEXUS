import styled from "styled-components";

const CustomTooltip = ({ active, payload }: any) => {
  if (active && payload && payload.length) {
    const data = payload[0].payload;

    return (
      <TooltipContainer>
        <p><strong>Time:</strong> {new Date(data.timestamp_measurement).toLocaleString()}</p>
        <p><strong>Battery Current:</strong> {data.BatteryCurrent} A</p>
        <p><strong>Battery Voltage:</strong> {data.BatteryVoltage} V</p>
        <p><strong>CPU Temperature:</strong> {data.TemperaturCPU} °C</p>
        <p><strong>Motherboard Temperature:</strong> {data.TemperaturMotherboard} °C</p>
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

export default CustomTooltip;
