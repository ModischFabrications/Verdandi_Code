"use strict";
const d = document;
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
    brightness: 127,
    showHours: true,
    showMinutes: true,
    showSeconds: true,
    colorH: [255, 0, 0],
    colorM: [0, 255, 0],
    colorS: [0, 0, 255],
    pollInterval: 10,
    nightmode: false,
    turnOffAt: `11:00`,
    turnOnAt: `12:00`,
    timezone: 'GMT0',
    timezoneName: 'GMT'
};
let timezones = {};
let currentTime = new Date(2000, 1, 1, 0, 0, 0, 0);
let timeUpdateTimeout;

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

    httpRequests.loadTimezones();
}

let httpRequests = {
    loadTimezones: function () {
        let self = this;
        let xhttp = new XMLHttpRequest();
        xhttp.open('GET', d.URL + 'timezones.json', true);
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                // load config values after timezones were loaded
                self.loadConfigValues();

                timezones = JSON.parse(xhttp.responseText);
            }
        }
        xhttp.send();
    },
    loadConfigValues: function () {
        let self = this;
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", d.URL + "config", true);
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                // send request to currently displayed time
                self.loadCurrentTime();

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
                    config.nightmode = res.nightmode;
                    console.log(res.turnOffAt)
                    config.turnOffAt =
                        `${
                            res.turnOffAt[0].toString().padStart(2, '0')
                        }:${
                            res.turnOffAt[1].toString().padStart(2, '0')
                        }`;
                    config.turnOnAt =
                        `${
                            res.turnOnAt[0].toString().padStart(2, '0')
                        }:${
                            res.turnOnAt[1].toString().padStart(2, '0')
                        }`;
                    config.timezone = res.timezone;
                    config.timezoneName = res.timezoneName;

                    timezoneDisplay.lastSelectedTzName = config.timezoneName;

                    // update ui according to loaded configuration and unblur site
                    updateUIElements();
                } catch { }
                d.body.classList.remove("blurred");
            }
        };
        xhttp.send();
    },
    dataUpdateTimeout: null,
    sendDataUpdate: function () {
        // clear existing timeout and set new one to send only every 200 ms
        clearTimeout(this.dataUpdateTimeout);
        this.dataUpdateTimeout = setTimeout(() => {
            let urlString = generateUrlString();

            let xhttp = new XMLHttpRequest();
            xhttp.open("POST", d.URL + "update", true);
            xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    httpRequests.loadCurrentTime();
                }
            }
            xhttp.send(urlString);
        }, 100);

        function generateUrlString() {
            console.log(config);
            // `brightness=${config.brightness}&showHours=${config.showHours}&showMinutes=${config.showMinutes}&showSeconds=${config.showSeconds}&colorH_R=${config.colorH[0]}&colorH_G=${config.colorH[1]}&colorH_B=${config.colorH[2]}&colorM_R=${config.colorM[0]}&colorM_G=${config.colorM[1]}&colorM_B=${config.colorM[2]}&colorS_R=${config.colorS[0]}&colorS_G=${config.colorS[1]}&colorS_B=${config.colorS[2]}&pollInterval=${config.pollInterval}&nightmode=${config.nightmode}&timezone=${config.timezone}`;

            let urlString = '';
            for (let [key, value] of Object.entries(config)) {
                urlString += `${key}=${value}&`;
            }
            urlString = urlString.substring(0, urlString.length - 1);

            return urlString;
        }
    },
    loadCurrentTime: function () {
        let xhttp = new XMLHttpRequest();
        xhttp.open('GET', d.URL + 'time');
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                let time = JSON.parse(xhttp.responseText);
                currentTime = new Date(2000, 1, 1, time.hours || 0, time.minutes || 0, time.seconds || 0, 0);

                // set timeout to 900 ms to cope for response time
                clearTimeout(timeUpdateTimeout);
                timeUpdateTimeout = setTimeout(updateTime, 900);
            } else {
                let timeEl = d.getElementById('timeDisplay');
                timeEl.innerHTML = `--:--:--`;
            }
        }
        xhttp.send();
    }
}

// object to handle timezone selection (generate and handle ui)
let timezoneDisplay = {
    currentTimezonePath: [],
    lastSelectedTzName: 'GMT',
    update: function () {
        let tzToDisplay = this.getChildrenInCurrentTimezonePath();
        let tzElements = this.generateTzHtmlList(tzToDisplay);

        // remove existing children and add newly generated ones
        while (tzSelectionContainer.firstChild) {
            tzSelectionContainer.removeChild(tzSelectionContainer.firstChild);
        }
        tzSelectionContainer.appendChild(tzElements);
    },
    // goes through the timezones object according to the current timezone path
    // returns: divisions found in the path
    getChildrenInCurrentTimezonePath: function () {
        let tempPart = timezones;
        this.currentTimezonePath.forEach(function (tzName, index) {
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
    },
    // generates a list of html elements
    // timezoneArray: a map of timezones
    // returns: an html list containing al elements from the timezoneArray
    generateTzHtmlList: function (timezoneArray) {
        // TODO: add search bar
        let ul = createDomElement('ul', ['dropdown-menu']);
        let sortedTimezoneArray = sortJsonByKey(timezoneArray);

        // hack to check if the first layer of the two jsons are equal
        // add back arrow if not
        if (Object.keys(sortedTimezoneArray)[0] != Object.keys(sortJsonByKey(timezones.divisions))[0]) {
            ul.appendChild(this.createBackArrow());
        }

        // for every entry of the sorted arry of time zones add a new list item
        for (const tz of Object.entries(sortedTimezoneArray)) {
            ul.appendChild(this.createTzListItem(tz[0], tz[1]));
        }

        return ul;
    },
    // returns: left arrow html element for going one layer up in the timezone search
    createBackArrow: function () {
        let i = createDomElement('i', ['arrow', 'left']);
        let d = createDomElement('div', ['back-button-container'], [i]);
        let backArrow = createDomElement('li', [], [d]);
        backArrow.addEventListener('click', this.removeNameFromTzPath);
        return backArrow;
    },
    // creates one list item for the timezone display
    // tzKey: name of the timezone
    // tzValue: timezone definition according to IANA
    // returns: html item of one timezone element
    createTzListItem: function (tzKey, tzValue) {
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
            let self = this;
            li.addEventListener('click', function () {
                self.addNameToTzPath(tzKey);
            });
        } else {
            // get tzString from tz value
            let tzStringDefinition = getTzStringFromTzValue(tzValue);

            // highlight if it's the currently selected tz
            if (config.timezoneName == tzStringDefinition) li.classList.add('highlighted-item');

            // update the config and send the updated data
            li.addEventListener('click', function () {
                // if the tz is an object with the same time zones take the first TZ identifier
                onValueChange([tzKey, tzStringDefinition], 'timezone');
            });
        }

        return li;

        // returns: divisions object if present, else the passed key
        function getTzStringFromTzValue(value) {
            if (typeof (value) == 'object') {
                let subDivisions = value.divisions;
                return Object.values(subDivisions)[0];
            }
            return value;
        }
    },
    addNameToTzPath: function (tzName) {
        timezoneDisplay.currentTimezonePath.push(tzName);
        // updateTimezoneDisplay();
        timezoneDisplay.update();
    },
    removeNameFromTzPath: function () {
        timezoneDisplay.currentTimezonePath.pop();
        // updateTimezoneDisplay();
        timezoneDisplay.update();
    },
    toggleShowSelection: function () {
        // reset the path to the last selected time zone
        timezoneDisplay.currentTimezonePath = [];
        // updateTimezoneDisplay();
        timezoneDisplay.update();

        // hide/show the time zone list + turn arrow
        tzSelectionButton.getElementsByClassName('arrow')[0].classList.toggle('right');
        tzSelectionButton.getElementsByClassName('arrow')[0].classList.toggle('down');
        tzSelectionContainer.classList.toggle('hidden');
    }
}

function updateTime() {
    let timeEl = d.getElementById('timeDisplay');
    currentTime = new Date(currentTime.getTime() + 1000);

    let h = currentTime.getHours().toString().padStart(2, '0'),
        m = currentTime.getMinutes().toString().padStart(2, '0'),
        s = currentTime.getSeconds().toString().padStart(2, '0');
    timeEl.innerHTML = `${h}:${m}:${s}`;

    timeUpdateTimeout = setTimeout(updateTime, 1000);
}

function sortJsonByKey(unordered) {
    const ordered = {};

    Object.keys(unordered).sort().forEach(function (key) {
        ordered[key] = unordered[key];
    });

    return ordered;
}

function onValueChange(element, targetVar) {
    /* possibly remove switch statement to update everything all the time to remove characters */
    switch (targetVar) {
        case 'brightness':
            brightnessOutput.value = element.value;
            config.brightness = parseInt(element.value);
            break;
        case 'showHours':
        case 'showMinutes':
        case 'showSeconds':
            console.log(element.checked);
            config[targetVar] = element.checked;
            toggleHiddenById(element.getAttribute('for'));
            break;
        case 'colorH':
        case 'colorM':
        case 'colorS':
            config[targetVar] = [
                parseInt(element.value.slice(1, 3), 16),
                parseInt(element.value.slice(3, 5), 16),
                parseInt(element.value.slice(5, 7), 16)
            ];
            break;
        case 'pollInterval':
            config.pollInterval = Math.min(Math.max(element.value, 0), 100000);
            break;
        case 'nightmode':
            config.nightmode = element.checked;
            d.getElementById('turnOffAt').toggleAttribute('disabled');
            d.getElementById('turnOnAt').toggleAttribute('disabled');
            break;
        case 'turnOffAt':
            config.turnOffAt = element.value;
            break;
        case 'turnOnAt':
            config.turnOnAt = element.value;
            break;
        case 'timezone':
        case 'timezoneName':
            config.timezone = element[1];
            config.timezoneName = element[0];
            timezoneDisplay.lastSelectedTzName = element[0];
            timezoneDisplay.toggleShowSelection();
            break;
        default:
            break;
    }
    updateUIElements();
    httpRequests.sendDataUpdate();
}

function updateUIElements() {
    brightenessEl.value = config.brightness;
    brightnessOutput.value = config.brightness;
    showHoursEl.checked = config.showHours;
    if (config.showHours) {
        d.getElementById('colorH').classList.remove('showDisabled');
        d.getElementById('colorH').removeAttribute('disabled');
    } else {
        d.getElementById('colorH').classList.add('showDisabled');
        d.getElementById('colorH').setAttributeNode(d.createAttribute('disabled'));
    }
    showMinutesEl.checked = config.showMinutes;
    if (config.showMinutes) {
        d.getElementById('colorM').classList.remove('showDisabled');
        d.getElementById('colorM').removeAttribute('disabled');
    } else {
        d.getElementById('colorM').classList.add('showDisabled');
        d.getElementById('colorM').setAttributeNode(d.createAttribute('disabled'));
    }
    showSecondsEl.checked = config.showSeconds;
    if (config.showSeconds) {
        d.getElementById('colorS').classList.remove('showDisabled');
        d.getElementById('colorS').removeAttribute('disabled');
    } else {
        d.getElementById('colorS').classList.add('showDisabled');
        d.getElementById('colorS').setAttributeNode(d.createAttribute('disabled'));
    }
    hourColorEl.value =
        '#' +
        get2DigitHex(config.colorH[0]) +
        get2DigitHex(config.colorH[1]) +
        get2DigitHex(config.colorH[2]);
    minuteColorEl.value =
        '#' +
        get2DigitHex(config.colorM[0]) +
        get2DigitHex(config.colorM[1]) +
        get2DigitHex(config.colorM[2]);
    secondColorEl.value =
        '#' +
        get2DigitHex(config.colorS[0]) +
        get2DigitHex(config.colorS[1]) +
        get2DigitHex(config.colorS[2]);
    pollingEl.value = config.pollInterval;
    d.getElementsByClassName('tz-input')[0].value = config.timezoneName;
    d.getElementById('nightmode').checked = config.nightmode;
    if (config.nightmode) {
        d.getElementById('turnOffAt').classList.remove('showDisabled');
        d.getElementById('turnOffAt').removeAttribute('disabled');
        d.getElementById('turnOnAt').classList.remove('showDisabled');
        d.getElementById('turnOnAt').removeAttribute('disabled');
    } else {
        d.getElementById('turnOffAt').classList.add('showDisabled');
        d.getElementById('turnOffAt').setAttributeNode(d.createAttribute('disabled'));
        d.getElementById('turnOnAt').classList.add('showDisabled');
        d.getElementById('turnOnAt').setAttributeNode(d.createAttribute('disabled'));
    }
    d.getElementById('turnOffAt').value = config.turnOffAt;
    d.getElementById('turnOnAt').value = config.turnOnAt;
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
