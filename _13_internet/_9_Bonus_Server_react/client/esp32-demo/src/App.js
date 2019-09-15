import React from "react";
import logo from "./logo.svg";
import "./App.css";
import "react-vis/dist/style.css";
import {
  XYPlot,
  LineSeries,
  VerticalGridLines,
  HorizontalGridLines,
  YAxis
} from "react-vis";

const baseURl = "";

function App() {
  const [ledVal, setLedVal] = React.useState(0);
  const [tempData, setTempData] = React.useState([
    { x: 1, y: 0 },{ x: 2, y: 0 },{ x: 3, y: 0 },{ x: 4, y: 0 },{ x: 5, y: 0 },{ x: 6, y: 0 },{ x: 7, y: 0 },{ x: 8, y: 0 },
  ]);

  React.useEffect(() => {
    const handler = setInterval(() => {
      setNewTemp();
    }, 1000);
    return () =>{
      clearInterval(handler);
    }
  }, [tempData]);

  const setNewTemp = async () => {
    const tempResponse = await fetch(`${baseURl}/api/temperature`);
    const tempJson = await tempResponse.json();
    let newData = tempData.length > 8 ? tempData.slice(1) : tempData;
    
    newData.push({
      x: newData[newData.length - 1].x + 1,
      y: tempJson.temperature
    });
    setTempData(newData);
    console.log("here",  newData);
  };

  const toggleLight = async () => {
    var newVal = ledVal === 0 ? 1 : 0;
    setLedVal(newVal);
    fetch(`${baseURl}/api/led`, {
      method: "POST",
      body: JSON.stringify({ led_on: ledVal })
    });
  };

  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <div
          style={{ backgroundColor: ledVal === 1 ? "white" : "#61dafb" }}
          className="led"
          onClick={toggleLight}
        />
        <XYPlot height={300} width={300}>
          <YAxis tickFormat={v => v.toFixed(2)} />
          <LineSeries data={tempData} animation={"smooth"} />
          <VerticalGridLines />
          <HorizontalGridLines />
        </XYPlot>
        <p>ESP32 - React</p>
      </header>
    </div>
  );
}

export default App;
