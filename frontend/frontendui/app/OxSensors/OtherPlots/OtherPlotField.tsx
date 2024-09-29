"use client";

import React, { useEffect, useState } from "react";
import { GiChemicalTank, GiChemicalDrop, GiChemicalArrow, GiChemicalBolt } from 'react-icons/gi';
import { MdOutlineScience, MdOutlineSensors } from 'react-icons/md';
import { FaBars } from 'react-icons/fa';
import styles from "../oxStyles/PlotField.module.css";
import { OtherDataProps } from "./OtherPlotInterface";
import OtherPlot from "./OtherPlot";
import { get_latest_otherData } from "@/functions/get_latest_other";


interface InitialData {
  plotInitalData: OtherDataProps[];
}

export default function OtherPlotField({ plotInitalData }: InitialData) {

  const [OtherData, setOtherData] = useState<OtherDataProps[]>(plotInitalData);

  useEffect(() => {
    console.log(OtherData);
  }, [] ); 

  const fetchNewData = async () => {
    const lastData = OtherData[OtherData.length - 1];
    if (lastData && lastData._id) {
      const newData: [OtherDataProps] = await get_latest_otherData(lastData._id);
      console.log(newData);
      setOtherData((prevData) => [...prevData, ...newData]);
    } else {
      const firstData = await get_latest_otherData("0");
      console.log(firstData);
      setOtherData(firstData);
    }
  };

  useEffect(() => {
    const intervalId = setInterval(fetchNewData, 1000);
    
    

    return () => clearInterval(intervalId);
  }, [OtherData]);




  const [showPlot1, setShowPlot1] = useState(true);
  const [showPlot2, setShowPlot2] = useState(false);
  const [showPlot3, setShowPlot3] = useState(false);
  const [showPlot4, setShowPlot4] = useState(false);
  const [showPlot5, setShowPlot5] = useState(false);
  const [showPlot6, setShowPlot6] = useState(false);
  const [sidebarCollapsed, setSidebarCollapsed] = useState(false);

  const handleButtonClick = (sensorName: string) => {
    switch (sensorName) {
      case "Sensor1":
        setShowPlot1(!showPlot1);
        break;
      case "Sensor2":
        setShowPlot2(!showPlot2);
        break;
      case "Sensor3":
        setShowPlot3(!showPlot3);
        break;
      case "Sensor4":
        setShowPlot4(!showPlot4);
        break;
      case "Sensor5":
        setShowPlot5(!showPlot5);
        break;
      case "Sensor6":
        setShowPlot6(!showPlot6);
        break;
      default:
        break;
    }
  };

  const toggleSidebar = () => {
    setSidebarCollapsed(!sidebarCollapsed);
  };

  const [timeMeasurementFeat, setTimemeasurementFeat] = useState(false)

  const timeMeasurementFunc = () => { 
    setTimemeasurementFeat(!timeMeasurementFeat)
  }

  return (
    <div style={{ display: "flex", flexDirection: "row", height: "100%" }}>
      {sidebarCollapsed && (
        
        <button onClick={toggleSidebar} className={styles.toggleButton}>
          <FaBars />
        </button>
      )}
      <div className={`${styles.sidebar} ${sidebarCollapsed ? styles.collapsed : ''}`}>
        <h3>Sensor Plots</h3>
        <button onClick={toggleSidebar} style={{ alignSelf: "flex-end" }}>
          <FaBars />
        </button>
        <button onClick={(timeMeasurementFunc)}>Time Measurement</button>
        <button
          onClick={() => handleButtonClick("Sensor1")}
          className={showPlot1 ? styles.active : ""}
        >
          <MdOutlineSensors /> Sensor 1
        </button>
        <button
          onClick={() => handleButtonClick("Sensor2")}
          className={showPlot2 ? styles.active : ""}
        >
          <GiChemicalTank /> Sensor 2
        </button>
        <button
          onClick={() => handleButtonClick("Sensor3")}
          className={showPlot3 ? styles.active : ""}
        >
          <GiChemicalDrop /> Sensor 3
        </button>
        <button
          onClick={() => handleButtonClick("Sensor4")}
          className={showPlot4 ? styles.active : ""}
        >
          <GiChemicalArrow /> Sensor 4
        </button>
      
        
      </div>
      <div className={`${styles.content} ${sidebarCollapsed ? styles.expanded : ''}`}>

        {showPlot1 && <OtherPlot datakeyValue={"TemperaturMotherboard"} otherDataList={OtherData} measureTimeFeat={false} sensorname="Temperatur Motherboard"/> }
        {showPlot2 && <OtherPlot datakeyValue={"TemperaturCPU"} otherDataList={OtherData} measureTimeFeat={false} sensorname="TemperatureCPU"/> }
        {showPlot3 && <OtherPlot datakeyValue={"BatteryVoltage"} otherDataList={OtherData} measureTimeFeat={false} sensorname="BatteryVoltage"/> }
        {showPlot4 && <OtherPlot datakeyValue={"BatteryCurrent"} otherDataList={OtherData} measureTimeFeat={false} sensorname="BatteryCurrent"/> }
      
    
</div>

    </div>
  );
}
