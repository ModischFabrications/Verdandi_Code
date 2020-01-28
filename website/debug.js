let d = document;

function onload() {
    let xhttp = new XMLHttpRequest();
    xhttp.open('GET', d.URL + '/messages', true);
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let debugMessages = JSON.parse(xhttp.responseText);
            console.log(debugMessages)
        }
    }
    xhttp.send();
}