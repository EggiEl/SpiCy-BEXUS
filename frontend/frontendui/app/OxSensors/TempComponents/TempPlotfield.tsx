"use client";

import React, { useState } from "react";
import { GiChemicalTank, GiChemicalDrop, GiChemicalArrow, GiChemicalBolt } from 'react-icons/gi';
import { MdOutlineScience, MdOutlineSensors } from 'react-icons/md';
import { FaBars } from 'react-icons/fa';
import styles from "../oxStyles/PlotField.module.css";
import { OxygenSensorData } from "../OxInterfaces/OxygenSensorData"
import TempSensorData from "../TempInterfaces/TempsensorData"
import SensorPlotTemp from "./SensorPlotTemp"


interface InitialData {
  plotInitalData: TempSensorData[][];
}

export default function PlotField({ plotInitalData }: InitialData) {
  const TempSensor1 = plotInitalData[0];
  const TempSensor2 = plotInitalData[1];
  const TempSensor3 = plotInitalData[2];
  const TempSensor4 = plotInitalData[3];
  const TempSensor5 = plotInitalData[4];
  const TempSensor6 = plotInitalData[5];

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
        <button
          onClick={() => handleButtonClick("Sensor5")}
          className={showPlot5 ? styles.active : ""}
        >
          <GiChemicalBolt /> Sensor 5
        </button>
        <button
          onClick={() => handleButtonClick("Sensor6")}
          className={showPlot6 ? styles.active : ""}
        >
          <MdOutlineScience /> Sensor 6
        </button>
      </div>
      <div className={`${styles.content} ${sidebarCollapsed ? styles.expanded : ''}`}>
        {showPlot1 && <SensorPlotTemp initialData={TempSensor1} sensorname="Sensor1" measureTimeFeat={timeMeasurementFeat}  />}
        {showPlot2 && <SensorPlotTemp initialData={TempSensor2} sensorname="Sensor2" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot3 && <SensorPlotTemp initialData={TempSensor3} sensorname="Sensor3" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot4 && <SensorPlotTemp initialData={TempSensor4} sensorname="Sensor4" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot5 && <SensorPlotTemp initialData={TempSensor5} sensorname="Sensor5" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot6 && <SensorPlotTemp initialData={TempSensor6} sensorname="Sensor6" measureTimeFeat={timeMeasurementFeat}/>}
      </div>
    </div>
  );
}
