async function getTemperature() {
  const result = await fetch("/api/temperature");
  const temperature = await result.json();
  console.log(temperature);
  const el = document.getElementById("temperature-val");
  el.innerText = temperature.temperature;
}
setInterval(getTemperature, 1000);

let isLedOn = false;
async function toggleLed() {
  const el = document.getElementById("led-button");
  isLedOn = !isLedOn;
  fetch("api/led", { method: "POST", body: JSON.stringify({ isLedOn }) });
  if (isLedOn) {
    el.classList.add("led-on");
    el.classList.remove("led-off");
  } else {
    el.classList.add("led-off");
    el.classList.remove("led-on");
  }
}
