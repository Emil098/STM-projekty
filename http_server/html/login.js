/**
 * login.js
 * Client-side authentication handler for the embedded web interface.
 *
 * Handles login form submission and communication with the `/api/login`
 * endpoint exposed by the embedded device.
 *
 * The backend may respond in two different ways:
 *
 * 1) JSON response indicating login success or failure
 * 2) HTTP redirect (302) to the dashboard page
 *
 * Since the Fetch API does not automatically update the browser URL after
 * following redirects, the script performs a manual redirect when needed.
 */


/* =========================================================
 * API Endpoints
 * ========================================================= */

/** @brief Login endpoint exposed by the device. */
const API_LOGIN = "/api/login";


/* =========================================================
 * DOM Helpers
 * ========================================================= */

/**
 * @brief Shorthand for document.getElementById().
 *
 * @param {string} id DOM element identifier.
 * @return {HTMLElement|null} Element reference.
 */
function $(id){
    return document.getElementById(id);
}


/**
 * @brief Reads trimmed value from an input element.
 *
 * @param {string} id Input element ID.
 * @return {string} Input value.
 */
function getInputValue(id){
    const el = $(id);
    return el ? el.value.trim() : "";
}


/* =========================================================
 * DOM Elements
 * ========================================================= */

/** @brief Login form element. */
const form = $("loginForm");

/** @brief Error message container. */
const errorMsg = $("errorMsg");


/* =========================================================
 * Login Handler
 * ========================================================= */

/**
 * @brief Process login form submission.
 *
 * @param {SubmitEvent} e Form submission event.
 *
 * @details
 * Prevents default form behavior and performs asynchronous
 * authentication using the Fetch API.
 */
form.addEventListener("submit", async (e) => {

    /* Prevent default page reload */
    e.preventDefault();

    /* Clear previous error */
    errorMsg.textContent = "";

    /* Read credentials */
    const user = getInputValue("user");
    const pwd  = $("pass").value;

    try {

        /**
         * Send login request to the device.
         */
        const res = await fetch(API_LOGIN, {

            method: "POST",

            /**
             * Include cookies/session credentials.
             * Required for session-based authentication.
             */
            credentials: "same-origin",

            headers: {
                "Content-Type": "application/json"
            },

            body: JSON.stringify({ user, pwd }),

            /**
             * Allow fetch to follow HTTP redirects.
             */
            redirect: "follow"
        });


        /**
         * Determine response type.
         */
        const ct = (res.headers.get("content-type") || "").toLowerCase();


        /* =====================================================
         * JSON response (API style)
         * ===================================================== */

        if (ct.includes("application/json")) {

            const data = await res.json();

            if (data && data.status === false) {

                errorMsg.textContent = "Invalid username or password";
                return;
            }

            /* Successful JSON response */
            return;
        }


        /* =====================================================
         * Redirect response (typical embedded behavior)
         * ===================================================== */

        /**
         * When the device issues an HTTP redirect,
         * Fetch follows it internally but the browser
         * URL remains unchanged.
         *
         * Therefore we manually navigate to the final URL.
         */
        if (res.url && !res.url.endsWith(API_LOGIN)) {

            window.location.assign(res.url);
            return;
        }


        /* =====================================================
         * Optional fallback for direct HTML response
         * ===================================================== */

        /*
        const html = await res.text();
        document.open();
        document.write(html);
        document.close();
        */

    }
    catch (err) {

        /**
         * Network error (device unreachable, timeout, etc.).
         */
        console.error(err);

        errorMsg.textContent = "Connection error";
    }
});