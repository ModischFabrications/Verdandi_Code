#pragma once

#include <Arduino.h>

// TODO: make constant variable
//const __FlashStringHelper* content = F("Hallo\nWorld");

const __FlashStringHelper* getContent() {
    return F("<!DOCTYPE html><html><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0, shrink-to-fit=no\"><title>Verdandi Configuration</title><link href=\"https://fonts.googleapis.com/css?family=Montserrat:100,200,300,300i,400,400i,500,500i,600,700,700i,800,900&display=swap\" rel=\"stylesheet\"><style>*{font-family:Montserrat,'Gill Sans','Gill Sans MT',Calibri,'Trebuchet MS',sans-serif}.config-table td{padding:10px}</style> <script>function onload(){document.getElementById('brightnessOutput').value=document.getElementById('brightnessSlider').value;} function sendUpdatedData(){let brightness=document.getElementById('brightnessSlider').value;let colorH=document.getElementById('colorH').value;let colorM=document.getElementById('colorM').value;let colorS=document.getElementById('colorS').value;let pollInterval=document.getElementById('pollInterval').value;let timeZone=document.getElementById('timeZone').value;var xhttp=new XMLHttpRequest();xhttp.open(\"POST\",'http://192.168.0.27/update',true);xhttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded');xhttp.send(`b:${brightness}&cH=${colorH}&cM=${colorM}&cS=${colorS}&p=${pollInterval}&tz=${timeZone}`);}</script> </head><body onload=\"onload()\"><div style=\"margin: auto; width: fit-content; padding: 2rem; background-color: #efefef; height: 100%; \"><h1 style=\"border-radius: 1rem; margin: auto; width: fit-content; padding: 2rem; background: black; color: white; margin-bottom: 1rem; \">Verdandi Configuration Page</h1><div><table class=\"config-table\"><tbody><tr><td>Brightness:</td><td colspan=\"3\"> <input id=\"brightnessSlider\" type=\"range\" min=\"0\" max=\"100\" value=\"50\" step=\"5\" onmousemove=\"brightnessOutput.value = this.value\" onchange=\"brightnessOutput.value = this.value\" > <output id=\"brightnessOutput\"></output></td></tr><tr><td>Colors:</td><td> H <input id=\"colorH\" type=\"color\" value=\"#ff0000\"></td><td> M <input id=\"colorM\" type=\"color\" value=\"#00ff00\"></td><td> S <input id=\"colorS\" type=\"color\" value=\"#0000ff\"></td></tr><tr><td>Poll interval:</td><td colspan=\"3\"> <input id=\"pollInterval\" type=\"number\" max=\"600\" min=\"1\" value=\"10\"> min</td></tr><tr><td>Time zone:</td><td colspan=\"3\"> <select id=\"timeZone\" name=\"timezone\"><option value=\"cet\">CET (Central European)</option><option value=\"est\">EST (Eastern Standard)</option><option value=\"gmt\">GMT (Greenwich Mean)</option><option value=\"pst\">PST (Pacific Standard)</option> </select></td></tr></tbody></table> <button onclick=\"sendUpdatedData()\">Update</button></div></div></body></html>");
}
