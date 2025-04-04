// AQI Updates for the main webpage
async function updateAQI() {
  try {
    const response = await fetch("/api/aqi");
    const data = await response.json();
    document.getElementById("aqi-value").textContent = Math.round(data.aqi);
    document.getElementById("co-value").textContent = Math.round(data.co_ppm);
    document.getElementById("co2-value").textContent = Math.round(data.co2_ppm);
    document.getElementById("smoke-value").textContent = Math.round(data.smoke_ppm);
    document.getElementById("status").textContent = "";
  } catch (err) {
    document.getElementById("status").textContent = "Connection error";
  }
}

// Update AQI every 3 seconds
setInterval(updateAQI, 3000);
updateAQI();

// Install button logic
const installBtn = document.getElementById("install-btn");
const setupInstructions = document.getElementById("setup-instructions");

installBtn.addEventListener("click", async () => {
  const userAgent = navigator.userAgent.toLowerCase();
  if (/android/.test(userAgent)) {
    try {
      const iframe = document.createElement("iframe");
      iframe.style.display = "none";
      iframe.src = "aqi-widget://launch";
      document.body.appendChild(iframe);

      setTimeout(() => {
        document.body.removeChild(iframe);
        if (document.hasFocus()) {
          setupInstructions.classList.remove("hidden");
          alert("AQI Widget app not found. Please follow the setup instructions to install the app.");
        }
      }, 1000);
    } catch (err) {
      setupInstructions.classList.remove("hidden");
      alert("AQI Widget app not found. Please follow the setup instructions to install the app.");
    }
  } else {
    alert("This feature is only supported on Android devices.");
  }
});