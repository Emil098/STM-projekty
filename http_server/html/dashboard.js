/**
 * dashboard.js
 * UI polling + gauge rendering for STM32 DHT22 dashboard.
 *
 * - Periodically fetches JSON from /api/sensor
 * - Updates numeric readouts + status indicator
 * - Renders semi-circular SVG gauges using stroke-dasharray and needle rotation
 */

// ====== CONFIG ======
const API_URL = "/api/sensor";
const PERIOD_MS = 1000;

// Mapping ranges used to normalize values to <0..1>
const TEMP_MIN = -40, TEMP_MAX = 80;
const HUM_MIN  = 0,   HUM_MAX  = 100;

// ====== HELPERS ======
function clamp(x, lo, hi) { return Math.min(hi, Math.max(lo, x)); }
function lerp(a, b, t)    { return a + (b - a) * t; }

// Cache DOM references (avoid repeated getElementById in hot path)
const el = {
  dot:        document.getElementById("dot"),
  statusText: document.getElementById("statusText"),
  tVal:       document.getElementById("tVal"),
  hVal:       document.getElementById("hVal"),
  tHint:      document.getElementById("tHint"),
  hHint:      document.getElementById("hHint"),
  tRange:     document.getElementById("tRange"),
  hRange:     document.getElementById("hRange"),
  tArc:       document.getElementById("tArc"),
  hArc:       document.getElementById("hArc"),
  tNeedle:    document.getElementById("tNeedle"),
  hNeedle:    document.getElementById("hNeedle"),
};

// Cache arc lengths once (SVG path length is constant)
const arcLen = {
  t: el.tArc.getTotalLength(),
  h: el.hArc.getTotalLength(),
};

/**
 * @brief Set gauge arc fill using stroke-dasharray.
 * @param {SVGPathElement} path
 * @param {number} L Total path length
 * @param {number} t01 Normalized value in range <0..1>
 */
function setArc(path, L, t01) {
  const val = clamp(t01, 0, 1) * L;
  path.style.strokeDasharray = `${val} 9999`;
}

/**
 * @brief Rotate gauge needle based on normalized value.
 *
 * Needle rotation range:
 *  -90° (left) -> +90° (right)
 *
 * Pivot point is taken from the SVG group translation currently present
 * in the DOM (translate(cx cy)). This avoids hardcoding 110/120.
 */
function setNeedle(needleGroup, t01) {
  const deg = lerp(-90, 90, clamp(t01, 0, 1));

  // Read the existing translate(cx cy) from the element's current transform.
  // Expected format: "translate(cx cy) rotate(x)"
  const tr = needleGroup.getAttribute("transform") || "translate(110 120) rotate(0)";
  const m = tr.match(/translate\(\s*([-\d.]+)\s+([-\d.]+)\s*\)/);

  const cx = m ? m[1] : 110;
  const cy = m ? m[2] : 120;

  needleGroup.setAttribute("transform", `translate(${cx} ${cy}) rotate(${deg})`);
}

function setStatus(ok, text) {
  el.dot.classList.toggle("ok", ok);
  el.dot.classList.toggle("bad", !ok);
  el.statusText.textContent = text;
}

function classifyTemp(t) {
  if (t < 18) return "Cold";
  if (t < 26) return "Normal";
  if (t < 32) return "Warm";
  return "Hot";
}

function classifyHum(h) {
  if (h < 50) return "Dry";
  if (h < 75) return "Normal";
  return "Wet";
}

/**
 * @brief Reset dashboard readouts and gauges to an "unknown" state.
 *
 * @details
 * Used when sensor data is unavailable or invalid (HTTP/JSON errors).
 * Prevents showing NaN and makes the UI deterministic in error scenarios.
 */
function resetUI() {
  // Numeric values: em dash is a common "no data" placeholder
  el.tVal.textContent = "--.-";
  el.hVal.textContent = "--.-";

  // Hints / classifications
  el.tHint.textContent = "N/A";
  el.hHint.textContent = "N/A";

  // Gauges: clear fill and set needle to the leftmost position
  setArc(el.tArc, arcLen.t, 0);
  setNeedle(el.tNeedle, 0);

  setArc(el.hArc, arcLen.h, 0);
  setNeedle(el.hNeedle, 0);
}

/**
 * @brief Update dashboard UI and gauges using current readings.
 */
function updateUI(temp, hum) {
  el.tVal.textContent = Number(temp).toFixed(1);
  el.hVal.textContent = Number(hum).toFixed(1);

  el.tHint.textContent = classifyTemp(temp);
  el.hHint.textContent = classifyHum(hum);

  // Normalize values to <0..1> to drive gauges
  const t01 = (temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
  const h01 = (hum  - HUM_MIN)  / (HUM_MAX  - HUM_MIN);

  setArc(el.tArc, arcLen.t, t01);
  setNeedle(el.tNeedle, t01);

  setArc(el.hArc, arcLen.h, h01);
  setNeedle(el.hNeedle, h01);

  setStatus(true, "Connected - OK");
}

/**
 * @brief Fetch sensor data from device REST endpoint.
 *
 * Expected JSON format:
 *  { "temp": <number>, "hum": <number> }
 *
 * Also accepts alternative key names (temperature/t, humidity/h),
 * which is useful during firmware iterations.
 */
async function fetchSensor() {
  const res = await fetch(API_URL, { cache: "no-store" });
  if (!res.ok) throw new Error(`HTTP ${res.status}`);

  const data = await res.json();

  const temp = (data.temp ?? data.temperature ?? data.t);
  const hum  = (data.hum  ?? data.humidity    ?? data.h);

  if (typeof temp !== "number" || typeof hum !== "number")
    throw new Error("Invalid JSON format (expecting temp/hum as number)");

  updateUI(temp, hum);
}

// ====== LOOP ======
let timer = null;

async function pollOnce() {
  try {
    await fetchSensor();
  } catch (e) {
    setStatus(false, `Data missing - ${e.message}`);
    resetUI();
  }
}

function start() {
  // Static range labels
  el.tRange.textContent = `${TEMP_MIN}…${TEMP_MAX} °C`;
  el.hRange.textContent = `${HUM_MIN}…${HUM_MAX} %RH`;

  pollOnce();
  timer = setInterval(pollOnce, PERIOD_MS);
}

start();