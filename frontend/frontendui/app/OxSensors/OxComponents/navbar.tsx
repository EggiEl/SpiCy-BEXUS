"use client";

import styled from 'styled-components';

const NavbarWrapper = styled.div`
    display: flex;
    justify-content: space-around;
    align-items: center;
    align-content: center;
    padding: 20px;
    width: 100%;
    color: white;
   
    background-color: #333;
    position: fixed;
`;

const Button = styled.button<{ isActive: boolean }>`
    background-color: ${({ isActive }) => (isActive ? 'green' : 'transparent')};
    color: ${({ isActive }) => (isActive ? 'white' : 'inherit')};
    border: none;
    padding: 10px 20px;
    cursor: pointer;
    &:hover {
        background-color: ${({ isActive }) => (isActive ? 'darkgreen' : '#555')};
    }
`;

interface NavbarProps {
   activePlot: number; 
   setActivePlot: (value: number) => void;  
}

export default function Navbar({ activePlot, setActivePlot }: NavbarProps) {
    return (
        <NavbarWrapper>
            <Button onClick={() => setActivePlot(1)} isActive={activePlot === 1}>
                OxSensors
            </Button>
            <Button onClick={() => setActivePlot(2)} isActive={activePlot === 2}>
                TempSensors
            </Button>
            <Button onClick={() => setActivePlot(3)} isActive={activePlot === 3}> 
                Others
            </Button>
        </NavbarWrapper>
    );
}
