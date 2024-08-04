"use client";

import React, { useState } from "react";
import SensorPlotOx from "./SensorPlotOx";
import { GiChemicalTank, GiChemicalDrop, GiChemicalArrow, GiChemicalBolt } from 'react-icons/gi';
import { MdOutlineScience, MdOutlineSensors } from 'react-icons/md';
import { FaBars } from 'react-icons/fa';
import styles from "../oxStyles/PlotField.module.css";

interface OxygenSensorData {
  _id: string;
  fullstruct_id: string;
  percentOtwo: number;
  timestamp_measurement: string;
}

interface InitialData {
  plotInitalData: OxygenSensorData[][];
}

export default function PlotField({ plotInitalData }: InitialData) {
  const Oxsensor1 = plotInitalData[0];
  const Oxsensor2 = plotInitalData[1];
  const Oxsensor3 = plotInitalData[2];
  const Oxsensor4 = plotInitalData[3];
  const Oxsensor5 = plotInitalData[4];
  const Oxsensor6 = plotInitalData[5];

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
        {showPlot1 && <SensorPlotOx initialData={Oxsensor1} sensorname="Sensor1" measureTimeFeat={timeMeasurementFeat}  />}
        {showPlot2 && <SensorPlotOx initialData={Oxsensor2} sensorname="Sensor2" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot3 && <SensorPlotOx initialData={Oxsensor3} sensorname="Sensor3" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot4 && <SensorPlotOx initialData={Oxsensor4} sensorname="Sensor4" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot5 && <SensorPlotOx initialData={Oxsensor5} sensorname="Sensor5" measureTimeFeat={timeMeasurementFeat}/>}
        {showPlot6 && <SensorPlotOx initialData={Oxsensor6} sensorname="Sensor6" measureTimeFeat={timeMeasurementFeat}/>}
      </div>
    </div>
  );
}
