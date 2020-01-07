"use strict";
let d = document;
let brightenessEl,
    showHoursEl,
    showMinutesEl,
    showSecondsEl,
    hourColorEl,
    minuteColorEl,
    secondColorEl,
    pollingEl;
let config = {
    brightness: 50,
    showHours: true,
    showMinutes: true,
    showSeconds: true,
    colorH: [255, 0, 0],
    colorM: [255, 0, 0],
    colorS: [255, 0, 0],
    pollInterval: 10
};

function toggleHiddenById(id) {
    d.getElementById(id).classList.toggle("showDisabled");
    d.getElementById(id).toggleAttribute("disabled");
}
function onload() {
    document.body.classList.remove("blurred");
    brightenessEl = d.getElementById("brightnessSlider");
    showHoursEl = d.getElementById("showHours");
    showMinutesEl = d.getElementById("showMinutes");
    showSecondsEl = d.getElementById("showSeconds");
    hourColorEl = d.getElementById("colorH");
    minuteColorEl = d.getElementById("colorM");
    secondColorEl = d.getElementById("colorS");
    pollingEl = d.getElementById("pollInterval");

    /* init value of brightness slider output */
    // brightnessOutput.value = brightenessEl.value;

    loadConfigValues();
}
function loadConfigValues() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", d.URL + "config", true);
    xhttp.send();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            /* sucessful response */
            let res = JSON.parse(xhttp.responseText);
            console.log(xhttp.responseText);
            console.log(res);
            config.brightness = parseInt(res.brightness);
            config.showHours = res.showHours;
            config.showMinutes = res.showMinutes;
            config.showSeconds = res.showSeconds;
            config.colorH = res.colorH;
            config.colorM = res.colorM;
            config.colorS = res.colorS;
            config.pollInterval = parseInt(res.pollInterval);

            updateUIElements();
        }
    };
}
function sendUpdatedData() {
    // TODO: send only every 200 ms
    let urlString = generateUrlString();

    var xhttp = new XMLHttpRequest();
    xhttp.open("POST", d.URL + "update", true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(urlString);
}
function generateUrlString() {
    console.log(config);
    // TODO: create string from config variable
    return `brightness=${config.brightness}&showHours=${config.showHours}&showMinutes=${config.showMinutes}&showSeconds=${config.showSeconds}&colorH_R=${config.colorH[0]}&colorH_G=${config.colorH[1]}&colorH_B=${config.colorH[2]}&colorM_R=${config.colorM[0]}&colorM_G=${config.colorM[1]}&colorM_B=${config.colorM[2]}&colorS_R=${config.colorS[0]}&colorS_G=${config.colorS[1]}&colorS_B=${config.colorS[2]}&pollInterval=${config.pollInterval}`;
}
function onValueChange(htmlElement, targetVar) {
    /* possibly remove switch statement to update everything all the time to remove characters */
    switch (targetVar) {
        case "brightness":
            brightnessOutput.value = htmlElement.value;
            config.brightness = parseInt(htmlElement.value);
            break;
        case "showHours":
        case "showMinutes":
        case "showSeconds":
            config[targetVar] = !config[targetVar];
            toggleHiddenById(htmlElement.getAttribute("for"));
            break;
        case "colorH":
        case "colorM":
        case "colorS":
            config[targetVar] = [
                parseInt(htmlElement.value.slice(1, 3), 16),
                parseInt(htmlElement.value.slice(3, 5), 16),
                parseInt(htmlElement.value.slice(5, 7), 16)
            ];
            break;
        case "pollInterval":
            config.pollInterval = Math.min(Math.max(htmlElement.value, 0), 100000);
            break;
        default:
            break;
    }
    sendUpdatedData();
}
function updateUIElements() {
    brightenessEl.value = config.brightness;
    brightnessOutput.value = config.brightness;
    showHoursEl.checked = config.showHours;
    if (config.showHours) {
        d.getElementById("colorH").classList.remove("showDisabled");
        d.getElementById("colorH").removeAttribute("disabled");
    } else {
        d.getElementById("colorH").classList.add("showDisabled");
        d.getElementById("colorH").setAttributeNode(
            document.createAttribute("disabled")
        );
    }
    showMinutesEl.checked = config.showMinutes;
    if (config.showMinutes) {
        d.getElementById("colorM").classList.remove("showDisabled");
        d.getElementById("colorM").removeAttribute("disabled");
    } else {
        d.getElementById("colorM").classList.add("showDisabled");
        d.getElementById("colorM").setAttributeNode(
            document.createAttribute("disabled")
        );
    }
    showSecondsEl.checked = config.showSeconds;
    if (config.showSeconds) {
        d.getElementById("colorS").classList.remove("showDisabled");
        d.getElementById("colorS").removeAttribute("disabled");
    } else {
        d.getElementById("colorS").classList.add("showDisabled");
        d.getElementById("colorS").setAttributeNode(
            document.createAttribute("disabled")
        );
    }
    hourColorEl.value =
        "#" +
        get2DigitHex(config.colorH[0]) +
        get2DigitHex(config.colorH[1]) +
        get2DigitHex(config.colorH[2]);
    minuteColorEl.value =
        "#" +
        get2DigitHex(config.colorM[0]) +
        get2DigitHex(config.colorM[1]) +
        get2DigitHex(config.colorM[2]);
    secondColorEl.value =
        "#" +
        get2DigitHex(config.colorS[0]) +
        get2DigitHex(config.colorS[1]) +
        get2DigitHex(config.colorS[2]);
    pollingEl.value = config.pollInterval;
}
function get2DigitHex(value) {
    return ("00" + value.toString(16).toUpperCase()).slice(-2);
}
