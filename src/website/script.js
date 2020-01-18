"use strict";
let d = document;
let brightenessEl,
    showHoursEl,
    showMinutesEl,
    showSecondsEl,
    hourColorEl,
    minuteColorEl,
    secondColorEl,
    pollingEl,
    tzSelectionContainer,
    tzSelectionButton;
let config = {
    brightness: 50,
    showHours: true,
    showMinutes: true,
    showSeconds: true,
    colorH: [255, 0, 0],
    colorM: [0, 255, 0],
    colorS: [0, 0, 255],
    pollInterval: 10,
    timezone: ''
};
let timezones = {};
let currentTimezonePath = [];
let lastSelectedTzName = '';

function toggleHiddenById(id) {
    d.getElementById(id).classList.toggle("showDisabled");
    d.getElementById(id).toggleAttribute("disabled");
}
function onload() {
    brightenessEl = d.getElementById("brightnessSlider");
    showHoursEl = d.getElementById("showHours");
    showMinutesEl = d.getElementById("showMinutes");
    showSecondsEl = d.getElementById("showSeconds");
    hourColorEl = d.getElementById("colorH");
    minuteColorEl = d.getElementById("colorM");
    secondColorEl = d.getElementById("colorS");
    pollingEl = d.getElementById("pollInterval");
    tzSelectionContainer = d.getElementsByClassName('tz-selection-container')[0];
    tzSelectionButton = d.getElementsByClassName('tz-selection-button')[0]

    // init value of brightness slider output
    // brightnessOutput.value = brightenessEl.value;

    loadTimezones();
}

function loadTimezones() {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', d.URL + 'timezones.json', true);
    xhttp.send();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            timezones = JSON.parse(xhttp.responseText);

            // load config values after timezones were loaded
            loadConfigValues();
        }
    }
}

function loadConfigValues() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", d.URL + "config", true);
    xhttp.send();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {

            // TODO: fix this try-catch...maybe send reset or something
            // provisorial try-catch to catch errors when json cannot be parsed
            try {
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
                config.timezone = res.timezone;

                lastSelectedTzName = getTzNameFromTzString(config.timezone, timezones);

                // update ui according to loaded configuration and unblur site
                updateUIElements();
            } catch {}
            d.body.classList.remove("blurred");
        }
    };
}

function getTzNameFromTzString(tzString, tzObject) {
    if (tzString == undefined) return '';
    for (const [tzKey, tzValue] of Object.entries(tzObject)) {
        if (typeof (tzValue) == 'string' && tzValue == tzString) return tzKey;

        if (typeof (tzValue) == 'object') {
            let temp = getTzNameFromTzString(tzString, tzValue);
            if (temp != '') return temp;
        }
    }

    return '';
}


function updateTimezoneDisplay() {
    let tzToDisplay = getChildrenInCurrentTimezonePath();
    let tzElements = generateTzHtmlList(tzToDisplay);

    // remove existing children and add newly generated ones
    while (tzSelectionContainer.firstChild) {
        tzSelectionContainer.removeChild(tzSelectionContainer.firstChild);
    }
    tzSelectionContainer.appendChild(tzElements);
}

function getChildrenInCurrentTimezonePath() {
    let tempPart = timezones;
    currentTimezonePath.forEach(function (tzName, index) {
        // check if current path has divisions object,
        // the time zone name is indeed part of the divisions,
        // and there are subdivisions within this selected part
        if ("divisions" in tempPart
            && tzName in tempPart.divisions
            && "divisions" in tempPart.divisions[tzName]) {
            tempPart = tempPart.divisions[tzName];
        } else {
            let msg = `Incorrect time zones where found while generating searching for a specific time zone.`;
            throw new Error(msg);
        }
    });

    return tempPart.divisions;
}

function generateTzHtmlList(timezoneArray) {
    // TODO: add search bar
    let ul = createDomElement('ul', ['dropdown-menu']);
    let sortedTimezoneArray = sortJsonByKey(timezoneArray);

    // hack to check if the first layer of the two jsons are equal
    // add back arrow if not
    if (Object.keys(sortedTimezoneArray)[0] != Object.keys(sortJsonByKey(timezones.divisions))[0]) {
        ul.appendChild(createNextArrow());
    }

    // for every entry of the sorted arry of time zones add a new list item
    for (const tz of Object.entries(sortedTimezoneArray)) {
        ul.appendChild(createTzListItem(tz[0], tz[1]));
    }

    return ul;

    // nested function definitions
    function createNextArrow() {
        let i = createDomElement('i', ['arrow', 'left']);
        let d = createDomElement('div', ['back-button-container'], [i]);
        let backArrow = createDomElement('li', [], [d]);
        backArrow.addEventListener('click', removeNameFromTzPath);
        return backArrow;
    }

    function createTzListItem(tzKey, tzValue) {
        // create the elements
        let tzItemName = createDomElement('span');
        tzItemName.innerHTML = tzKey;
        let li = createDomElement('li', [], [tzItemName]);

        // only if the value at array position 1 is an object and the containing timezones
        // do not have the same time make it clickable so that it add itself to the path
        if (typeof (tzValue) == 'object' && !tzValue.same_time) {

            let nextButton = createDomElement('i', ['arrow', 'right']);
            let nextSpan = createDomElement('span', ['next-button-span'], [nextButton]);
            li.appendChild(nextSpan);

            // go further down the list on click
            li.addEventListener('click', function () {
                addNameToTzPath(tzKey);
            });
        } else {
            // get tzString from tz value
            let tzStringDefinition = getTzStringFromTzValue();

            // highlight if it's the currently selected tz
            if (config.timezone == tzStringDefinition) li.classList.add('highlighted-item');

            // update the config and send the updated data
            li.addEventListener('click', function () {
                // if the tz is an object with the same time zones take the first TZ identifier
                onValueChange([tzKey, tzStringDefinition], 'timezone');
            });
        }

        return li;

        function getTzStringFromTzValue() {
            if (typeof (tzValue) == 'object') {
                let subDivisions = tzValue.divisions;
                let firstKey = Object.keys(subDivisions)[0];
                return subDivisions[firstKey];
            }
            return tzValue;
        }
    }
}

function showTzSelection() {
    // reset the path to the last selected time zone
    currentTimezonePath = [];
    updateTimezoneDisplay();

    // hide/show the time zone list + turn arrow
    tzSelectionButton.getElementsByClassName('arrow')[0].classList.toggle('right');
    tzSelectionButton.getElementsByClassName('arrow')[0].classList.toggle('down');
    tzSelectionContainer.classList.toggle('hidden');
}

function addNameToTzPath(tzName) {
    currentTimezonePath.push(tzName);
    updateTimezoneDisplay();
}

function removeNameFromTzPath() {
    currentTimezonePath.pop();
    updateTimezoneDisplay();
}

function sortJsonByKey(unordered) {
    const ordered = {};

    Object.keys(unordered).sort().forEach(function (key) {
        ordered[key] = unordered[key];
    });

    return ordered;
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
    return `brightness=${config.brightness}&showHours=${config.showHours}&showMinutes=${config.showMinutes}&showSeconds=${config.showSeconds}&colorH_R=${config.colorH[0]}&colorH_G=${config.colorH[1]}&colorH_B=${config.colorH[2]}&colorM_R=${config.colorM[0]}&colorM_G=${config.colorM[1]}&colorM_B=${config.colorM[2]}&colorS_R=${config.colorS[0]}&colorS_G=${config.colorS[1]}&colorS_B=${config.colorS[2]}&pollInterval=${config.pollInterval}&timezone=${config.timezone}`;
}

function onValueChange(element, targetVar) {
    /* possibly remove switch statement to update everything all the time to remove characters */
    switch (targetVar) {
        case "brightness":
            brightnessOutput.value = element.value;
            config.brightness = parseInt(element.value);
            break;
        case "showHours":
        case "showMinutes":
        case "showSeconds":
            config[targetVar] = !config[targetVar];
            toggleHiddenById(element.getAttribute("for"));
            break;
        case "colorH":
        case "colorM":
        case "colorS":
            config[targetVar] = [
                parseInt(element.value.slice(1, 3), 16),
                parseInt(element.value.slice(3, 5), 16),
                parseInt(element.value.slice(5, 7), 16)
            ];
            break;
        case "pollInterval":
            config.pollInterval = Math.min(Math.max(element.value, 0), 100000);
            break;
        case "timezone":
            config.timezone = element[1];
            lastSelectedTzName = element[0];
            showTzSelection();
            break;
        default:
            break;
    }
    updateUIElements();
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
            d.createAttribute("disabled")
        );
    }
    showMinutesEl.checked = config.showMinutes;
    if (config.showMinutes) {
        d.getElementById("colorM").classList.remove("showDisabled");
        d.getElementById("colorM").removeAttribute("disabled");
    } else {
        d.getElementById("colorM").classList.add("showDisabled");
        d.getElementById("colorM").setAttributeNode(
            d.createAttribute("disabled")
        );
    }
    showSecondsEl.checked = config.showSeconds;
    if (config.showSeconds) {
        d.getElementById("colorS").classList.remove("showDisabled");
        d.getElementById("colorS").removeAttribute("disabled");
    } else {
        d.getElementById("colorS").classList.add("showDisabled");
        d.getElementById("colorS").setAttributeNode(
            d.createAttribute("disabled")
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
    d.getElementsByClassName('tz-input')[0].value = lastSelectedTzName;
}

function get2DigitHex(value) {
    return ('00' + value.toString(16).toUpperCase()).slice(-2);
}

function createDomElement(tagName, classList = [], children = []) {
    let el = d.createElement(tagName);
    for (let cl of classList) {
        if (typeof (cl) == 'string') {
            el.classList.add(cl);
        }
    }
    for (let child of children) {
        if (typeof (child) == 'object') {
            el.appendChild(child);
        }
    }

    return el;
};
