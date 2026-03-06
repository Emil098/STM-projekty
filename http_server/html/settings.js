/**
 * setting.js
 * Settings page client-side controller (STM32 embedded web UI).
 *
 * Responsibilities:
 * - Read current configuration via REST endpoints (/api/net, /api/auth)
 * - Validate user input on the client side
 * - Submit changes via HTTP POST (JSON payload)
 * - Provide inline feedback messages for each section
 *
 * Intended for lightweight embedded HTTP servers.
 */


/* =========================================================
 * API Endpoints
 * ========================================================= */

/** REST endpoint for network configuration. */
const EP_NET  = "/api/net";

/** REST endpoint for authentication configuration. */
const EP_AUTH = "/api/auth";


/* =========================================================
 * DOM Helpers
 * ========================================================= */

/**
 * @brief Shorthand for getElementById().
 * @param {string} id DOM element id.
 * @return {HTMLElement|null} Resolved element reference or null if missing.
 */
function $(id){
  return document.getElementById(id);
}


/**
 * @brief Set an inline status message (info/success/error) for a section.
 *
 * @param {string} id Element id where the hint should be displayed.
 * @param {string} text Message text.
 * @param {number} col Message type: 1=info, 2=success, 3=error.
 */
function setHint(id, text, col = 1){

  const el = $(id);
  if (!el) return;

  el.textContent = text;

  /* Color is applied inline to keep this file independent from theme CSS decisions. */
  if (col === 1){
    el.style.color = "rgba(174,183,230,0.90)";
  }
  else if (col === 2){
    el.style.color = "rgba(9, 225, 189, 0.9)";
  }
  else{
    el.style.color = "rgba(255,92,122,0.95)";
  }
}


/* =========================================================
 * Validation
 * ========================================================= */

/**
 * @brief Lightweight IPv4 validator.
 *
 * @param {string} s Input string.
 * @return {boolean} True if the string matches IPv4 dotted-decimal format (0..255).
 *
 * @details
 * This validation is intentionally simple and fast.
 * It does not attempt to validate network semantics (e.g., reserved ranges).
 */
function isIPv4(s){

  const parts = s.trim().split(".");
  if (parts.length !== 4) return false;

  for (const p of parts){

    if (p === "" || p.length > 3) return false;
    if (!/^\d+$/.test(p)) return false;

    const n = Number(p);
    if (n < 0 || n > 255) return false;
  }

  return true;
}


/* =========================================================
 * HTTP Helpers
 * ========================================================= */

/**
 * @brief Perform GET request and parse JSON response.
 *
 * @param {string} url Target URL.
 * @return {Promise<any>} Parsed JSON object.
 * @throws {Error} When HTTP status is not OK.
 */
async function httpGetJson(url){

  const res = await fetch(url, { cache: "no-store" });
  if (!res.ok) throw new Error(`HTTP ${res.status}`);

  return await res.json();
}

/**
 * @brief Perform POST request with JSON payload.
 *
 * @param {string} url Target URL.
 * @param {Object} bodyObj JSON payload object.
 * @return {Promise<boolean>} True on success.
 * @throws {Error} When HTTP status is not OK.
 *
 * @details
 * Many embedded servers return empty bodies; this wrapper assumes
 * success is defined by HTTP status only.
 */
async function httpPostJson(url, bodyObj){

  const res = await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    cache: "no-store",
    body: JSON.stringify(bodyObj),
  });

  if (!res.ok) throw new Error(`HTTP ${res.status}`);
  return true;
}


/* =========================================================
 * Network Section
 * ========================================================= */

/**
 * @brief Load current network configuration and update form fields.
 * @return {Promise<void>}
 */
async function loadNetwork(){

  setHint("netHint", "Loading...");

  const data = await httpGetJson(EP_NET);

  $("ip").value   = data.ip   ?? "";
  $("mask").value = data.mask ?? "";
  $("gw").value   = data.gw   ?? "";

  setHint("netHint", "Loaded", 2);
}


/**
 * @brief Validate and save network configuration to the device.
 * @return {Promise<void>}
 *
 * @details
 * After applying network settings, the device IP may change.
 * The UI redirects the browser to the new address to reconnect.
 */
async function saveNetwork(){

  const ip   = $("ip").value.trim();
  const mask = $("mask").value.trim();
  const gw   = $("gw").value.trim();

  if (!isIPv4(ip))   return setHint("netHint", "Invalid IP address", 3);
  if (!isIPv4(mask)) return setHint("netHint", "Invalid netmask", 3);
  if (!isIPv4(gw))   return setHint("netHint", "Invalid gateway", 3);

  setHint("netHint", "Saving...");

  await httpPostJson(EP_NET, { ip, mask, gw });

  setHint("netHint", "Saved, reconnecting...", 2);

  /* Give the device a moment to reconfigure network before redirecting. */
  setTimeout(() => {
    window.location.href = `http://${ip}/settings.shtml`;
  }, 3000);
}


/* =========================================================
 * Authentication Section
 * ========================================================= */

/**
 * @brief Load authentication data and populate the username field.
 * @return {Promise<void>}
 *
 * @details
 * Password fields are always cleared to avoid exposing secrets in the UI.
 */
async function loadAuth(){

  setHint("authHint", "Loading...");

  const data = await httpGetJson(EP_AUTH);

  $("user").value  = data.user ?? "";
  $("pass").value  = "";
  $("pass2").value = "";

  setHint("authHint", "Loaded", 2);
}


/**
 * @brief Validate and save new credentials.
 * @return {Promise<void>}
 */
async function saveAuth(){

  const user  = $("user").value.trim();
  const pass  = $("pass").value;
  const pass2 = $("pass2").value;

  if (user.length < 1) return setHint("authHint", "Username is required", 3);
  if (pass.length < 4) return setHint("authHint", "Password must be at least 4 characters", 3);
  if (pass !== pass2)  return setHint("authHint", "Passwords do not match", 3);

  setHint("authHint", "Saving...");

  await httpPostJson(EP_AUTH, { user, pass });

  setHint("authHint", "Saved (re-login may be required)", 2);
}


/* =========================================================
 * Initialization
 * ========================================================= */

/**
 * @brief Bind UI events and load initial data from the device.
 * @return {Promise<void>}
 */
async function init(){

  $("btnNetReload")
    .addEventListener("click", () =>
      loadNetwork().catch(e => setHint("netHint", e.message, 3)));

  $("btnNetSave")
    .addEventListener("click", () =>
      saveNetwork().catch(e => setHint("netHint", e.message, 3)));

  $("btnAuthReload")
    .addEventListener("click", () =>
      loadAuth().catch(e => setHint("authHint", e.message, 3)));

  $("btnAuthSave")
    .addEventListener("click", () =>
      saveAuth().catch(e => setHint("authHint", e.message, 3)));

  /* Initial page load: populate both sections. */
  try{
    await loadNetwork();
    await loadAuth();
  }
  catch (e){
    /* Non-fatal: individual actions still work (Reload/Save) with per-section errors. */
  }
}


/** @brief Entry point. */
init();