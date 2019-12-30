#pragma once

#include <Arduino.h>

namespace Website {

// this needs to be a function because F-strings are disguised function calls
const __FlashStringHelper* getContent() {
    return F(
        "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' "
        "content='width=device-width,initial-scale=1.0, shrink-to-fit=no'><title>Verdandi "
        "Configuration</title><style>*{font-family:'Trebuchet "
        "MS',sans-serif}.width100{width:100%}.config-table "
        "td{padding:10px}.showDisabled{filter:grayscale(0.8) contrast(0.5);}</style> <script>'use "
        "strict';let d=document;let "
        "brightenessEl,showHoursEl,showMinutesEl,showSecondsEl,hourColorEl,minuteColorEl,"
        "secondColorEl,pollingEl;let "
        "config={brightness:50,showHours:true,showMinutes:true,showSeconds:true,colorH:[255,0,0],"
        "colorM:[255,0,0],colorS:[255,0,0],pollInterval:10};function "
        "toggleHiddenById(id){d.getElementById(id).classList.toggle('showDisabled');d."
        "getElementById(id).toggleAttribute('disabled');} function "
        "onload(){brightenessEl=d.getElementById('brightnessSlider');showHoursEl=d.getElementById('"
        "showHours');showMinutesEl=d.getElementById('showMinutes');showSecondsEl=d.getElementById('"
        "showSeconds');hourColorEl=d.getElementById('colorH');minuteColorEl=d.getElementById('"
        "colorM');secondColorEl=d.getElementById('colorS');pollingEl=d.getElementById('"
        "pollInterval');brightnessOutput.value=brightenessEl.value;loadConfigValues();} function "
        "loadConfigValues(){var xhttp=new "
        "XMLHttpRequest();xhttp.open('GET',d.URL+'config',true);xhttp.send();xhttp."
        "onreadystatechange=function(){if(this.readyState==4&&this.status==200){let "
        "res=JSON.parse(xhttp.responseText);console.log(xhttp.responseText);console.log(res);"
        "config.brightness=parseInt(res.brightness);config.showHours=res.showHours;config."
        "showMinutes=res.showMinutes;config.showSeconds=res.showSeconds;config.colorH=res.colorH;"
        "config.colorM=res.colorM;config.colorS=res.colorS;config.pollInterval=res.pollInterval;"
        "updateUIElements();}}} function sendUpdatedData(){let urlString=generateUrlString();var "
        "xhttp=new "
        "XMLHttpRequest();xhttp.open('POST',d.URL+'update',true);xhttp.setRequestHeader('Content-"
        "Type','application/x-www-form-urlencoded');xhttp.send(urlString);} function "
        "generateUrlString(){console.log(config);return`brightness=${config.brightness}&showHours=$"
        "{config.showHours}&showMinutes=${config.showMinutes}&showSeconds=${config.showSeconds}&"
        "colorH_R=${config.colorH[0]}&colorH_G=${config.colorH[1]}&colorH_B=${config.colorH[2]}&"
        "colorM_R=${config.colorM[0]}&colorM_G=${config.colorM[1]}&colorM_B=${config.colorM[2]}&"
        "colorS_R=${config.colorS[0]}&colorS_G=${config.colorS[1]}&colorS_B=${config.colorS[2]}&"
        "pollInterval=${config.pollInterval}`;} function "
        "onValueChange(htmlElement,targetVar){switch(targetVar){case'brightness':brightnessOutput."
        "value=htmlElement.value;config.brightness=parseInt(htmlElement.value);break;case'"
        "showHours':case'showMinutes':case'showSeconds':config[targetVar]=!config[targetVar];"
        "toggleHiddenById(htmlElement.getAttribute('for'));break;case'colorH':case'colorM':case'"
        "colorS':config[targetVar]=[parseInt(htmlElement.value.slice(1,3),16),parseInt(htmlElement."
        "value.slice(3,5),16),parseInt(htmlElement.value.slice(5,7),16),];break;case'pollInterval':"
        "config.pollInterval=Math.min(Math.max(htmlElement.value,0),10000);break;default:break;} "
        "sendUpdatedData();} function "
        "updateUIElements(){brightenessEl.value=config.brighteness;brightnessOutput.value=config."
        "brightness;showHoursEl.checked=config.showHours;if(config.showHours){d.getElementById('"
        "colorH').classList.remove('showDisabled');d.getElementById('colorH').removeAttribute('"
        "disabled');}else{d.getElementById('colorH').classList.add('showDisabled');d."
        "getElementById('colorH').setAttributeNode(document.createAttribute('disabled'));} "
        "showMinutesEl.checked=config.showMinutes;if(config.showMinutes){d.getElementById('colorM')"
        ".classList.remove('showDisabled');d.getElementById('colorM').removeAttribute('disabled');}"
        "else{d.getElementById('colorM').classList.add('showDisabled');d.getElementById('colorM')."
        "setAttributeNode(document.createAttribute('disabled'));} "
        "showSecondsEl.checked=config.showSeconds;if(config.showSeconds){d.getElementById('colorS')"
        ".classList.remove('showDisabled');d.getElementById('colorS').removeAttribute('disabled');}"
        "else{d.getElementById('colorS').classList.add('showDisabled');d.getElementById('colorS')."
        "setAttributeNode(document.createAttribute('disabled'));} "
        "hourColorEl.value='#'+get2DigitHex(config.colorH[0])+get2DigitHex(config.colorH[1])+"
        "get2DigitHex(config.colorH[2]);minuteColorEl.value='#'+get2DigitHex(config.colorM[0])+"
        "get2DigitHex(config.colorM[1])+get2DigitHex(config.colorM[2]);secondColorEl.value='#'+"
        "get2DigitHex(config.colorS[0])+get2DigitHex(config.colorS[1])+get2DigitHex(config.colorS["
        "2]);pollingEl.value=config.pollInterval;} function "
        "get2DigitHex(value){return('00'+value.toString(16).toUpperCase()).slice(-2);}</script> "
        "</head><body onload='onload()'><div style='margin: auto; width: fit-content; padding: "
        "2rem; background-color: #efefef; height: 100%; '><h1 style='border-radius: 1rem; margin: "
        "auto; width: fit-content; padding: 2rem; background: black; color: white; margin-bottom: "
        "1rem; '>Verdandi Configuration Page</h1><div><table class='config-table "
        "width100'><tbody><tr><td>Brightness:</td><td colspan='3'> <input id='brightnessSlider' "
        "type='range' min='0' max='255' value='127' oninput='onValueChange(this, \"brightness\")' "
        ">&nbsp; <output id='brightnessOutput'></output></td></tr><tr><td>Colors:</td><td> <input "
        "id='showHours' for='colorH' class='css-checkbox' onchange='onValueChange(this, "
        "\"showHours\")' type='checkbox' checked> <label for='colorH' "
        "class='css-label'>&nbsp;H&nbsp;</label> <input id='colorH' type='color' value='#ff0000' "
        "onchange='onValueChange(this, \"colorH\")'></td><td> <input id='showMinutes' for='colorM' "
        "class='css-checkbox' onchange='onValueChange(this, \"showMinutes\")' type='checkbox' "
        "checked> <label for='colorM' class='css-label'>&nbsp;M&nbsp;</label> <input id='colorM' "
        "type='color' value='#00ff00' onchange='onValueChange(this, \"colorM\")'></td><td> <input "
        "id='showSeconds' for='colorS' class='css-checkbox' onchange='onValueChange(this, "
        "\"showSeconds\")' type='checkbox' checked> <label for='colorS' "
        "class='css-label'>&nbsp;S&nbsp;</label> <input id='colorS' type='color' value='#0000ff' "
        "onchange='onValueChange(this, \"colorS\")'></td></tr><tr><td>Poll interval:</td><td "
        "colspan='3'> <input id='pollInterval' onchange='onValueChange(this, \"pollInterval\")' "
        "type='number' max='600' min='1' value='10'> "
        "min</td></tr></tbody></table></div></div></body></html>");
}
} // namespace Website
