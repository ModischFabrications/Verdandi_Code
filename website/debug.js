let d = document;

// on page load: request and display error and warning messages
function onload() {
    let xhttp = new XMLHttpRequest();
    xhttp.open('GET', d.URL + '/messages', true);
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let debugMessages = JSON.parse(xhttp.responseText);
            console.log('Debug messages: ', debugMessages)

            displayMessages(debugMessages);
        }
    }
    xhttp.send();
}

// show display messages in ui
function displayMessages(debugMessageObj) {
    let warningsContainer = d.getElementById('warningsContainer');
    let errorsContainer = d.getElementById('errorsContainer');
    if (!('warnings' in debugMessageObj) || !('errors' in debugMessageObj)) {
        warningsContainer.innerHTML = "Error loading debug messages."
        errorsContainer.innerHTML = "Error loading debug messages."
        return;
    }

    warningsContainer.innerHTML = '';
    errorsContainer.innerHTML = '';

    // display messaged depending on the current index in the loop
    for (let i = 0; i < debugMessageObj.warnings.length; ++i) {
        let index = (i + debugMessageObj.warningIndex) % debugMessageObj.warnings.length;
        let m = debugMessageObj.warnings[index];
        let newItem = d.createElement('li');
        newItem.classList.add('warning');
        newItem.classList.add('debugItem');
        newItem.innerHTML = m;
        warningsContainer.appendChild(newItem);
    }
    for (let i = 0; i < debugMessageObj.errors.length; ++i) {
        let index = (i + debugMessageObj.errorIndex) % debugMessageObj.errors.length;
        let m = debugMessageObj.errors[index];
        let newItem = d.createElement('li');
        newItem.classList.add('error');
        newItem.classList.add('debugItem');
        newItem.innerHTML = m;
        errorsContainer.appendChild(newItem);
    }
}