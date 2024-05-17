import { useState } from "react";

interface SensorButtonProps {
    sensorName : string ; 
    onClick : (sensorName : string) => void ; //Routing 
    maxVal : number ;
    minVal : number ;
    measuredData: number ; 
} 


export default function SensorButton(props: SensorButtonProps) {

    const { sensorName, maxVal, minVal, onClick, measuredData } = props;
    const [buttoncolor, setbuttonColor] = useState("green"); 
    if (measuredData > maxVal || measuredData < minVal){
        setbuttonColor("red");
    }
    else{
        setbuttonColor("green");
    }



    return (


        <button style={{"color" : "green"}} onClick={() => onClick(sensorName)}>{sensorName}</button>
    )
}

