let d = document;

// TODO: include warningsIndex and errorsIndex in JSON --> Ringbuffer

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

function displayMessages(debugMessageObject) {
    let warningsContainer = d.getElementById('warningsContainer');
    let errorsContainer = d.getElementById('errorsContainer');
    if (!('warnings' in debugMessageObject) || !('errors' in debugMessageObject)) {
        warningsContainer.innerHTML = "Error loading debug messages."
        errorsContainer.innerHTML = "Error loading debug messages."
        return;
    }

    warningsContainer.innerHTML = '';
    errorsContainer.innerHTML = '';

    for (let m of debugMessageObject.warnings) {
        let newItem = d.createElement('li');
        newItem.classList.add('warning');
        newItem.classList.add('debugItem');
        newItem.innerHTML = m;
        warningsContainer.appendChild(newItem);
    }
    for (let m of debugMessageObject.errors) {
        let newItem = d.createElement('li');
        newItem.classList.add('warning');
        newItem.classList.add('debugItem');
        newItem.innerHTML = m;
        errorsContainer.appendChild(newItem);
    }
}