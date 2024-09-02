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
    z-index: 1;
    background-color: #333;
    position: fixed;
    margin-inline: 20px;
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
    showOxPlot: () => void;
    showTempPlot: () => void;
    activePlot: 'ox' | 'temp';
}

export default function Navbar({ showOxPlot, showTempPlot, activePlot }: NavbarProps) {
    return (
        <NavbarWrapper>
            <Button onClick={showOxPlot} isActive={activePlot === 'ox'}>
                OxSensors
            </Button>
            <Button onClick={showTempPlot} isActive={activePlot === 'temp'}>
                TempSensors
            </Button>
        </NavbarWrapper>
    );
}
