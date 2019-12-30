#pragma once

#include <Arduino.h>

namespace Website {

// this needs to be a function because F-strings are disguised function calls
const __FlashStringHelper* getContent() {
    return F(
        "<!DOCTYPE html><html><head><meta charset=utf-8><meta "
        "content=\"width=device-width,initial-scale=1,shrink-to-fit=no\" "
        "name=viewport><title>Verdandi Configuration</title><style>*{font-family:'Trebuchet "
        "MS',sans-serif}.width100{width:100%}.config-table "
        "td{padding:10px}.showDisabled{filter:grayscale(.8) contrast(.5)}</style><script>\"use "
        "strict\";let d=document;let "
        "brightenessEl,showHoursEl,showMinutesEl,showSecondsEl,hourColorEl,minuteColorEl,"
        "secondColorEl,pollingEl;let "
        "config={brightness:50,showHours:true,showMinutes:true,showSeconds:true,colorH:[255,0,0],"
        "colorM:[255,0,0],colorS:[255,0,0],pollInterval:10};function "
        "toggleHiddenById(id){d.getElementById(id).classList.toggle(\"showDisabled\");d."
        "getElementById(id).toggleAttribute(\"disabled\");} function "
        "onload(){brightenessEl=d.getElementById(\"brightnessSlider\");showHoursEl=d."
        "getElementById(\"showHours\");showMinutesEl=d.getElementById(\"showMinutes\");"
        "showSecondsEl=d.getElementById(\"showSeconds\");hourColorEl=d.getElementById(\"colorH\");"
        "minuteColorEl=d.getElementById(\"colorM\");secondColorEl=d.getElementById(\"colorS\");"
        "pollingEl=d.getElementById(\"pollInterval\");brightnessOutput.value=brightenessEl.value;"
        "loadConfigValues();} function loadConfigValues(){var xhttp=new "
        "XMLHttpRequest();xhttp.open(\"GET\",d.URL+\"config\",true);xhttp.send();xhttp."
        "onreadystatechange=function(){console.log(this.readyState,this.status);if(this.readyState="
        "=4&&this.status==200){let "
        "res=JSON.parse(xhttp.responseText);console.log(xhttp.responseText);console.log(res);"
        "config.brightness=parseInt(res.brightness);config.showHours=res.show_hours=='1'?true:"
        "false;config.showMinutes=res.show_minutes=='1'?true:false;config.showSeconds=res.show_"
        "seconds=='1'?true:false;config.colorH=res.colorH;config.colorM=res.colorM;config.colorS="
        "res.colorS;config.pollInterval=res.poll_interval;updateUIElements();}}} function "
        "sendUpdatedData(){let urlString=generateUrlString();var xhttp=new "
        "XMLHttpRequest();xhttp.open(\"POST\",d.URL+\"update\",true);xhttp.setRequestHeader("
        "\"Content-Type\",\"application/x-www-form-urlencoded\");xhttp.send(urlString);} function "
        "generateUrlString(){console.log(config);return`brightness=${config.brightness}&show_hours="
        "${config.showHours}&show_seconds=${config.showMinutes}&show_seconds=${config.showSeconds}&"
        "colorH_R=${config.colorH[0]}&colorH_G=${config.colorH[1]}&colorH_B${config.colorH[2]}&"
        "colorM_R=${config.colorM[0]}&colorM_G=${config.colorM[1]}&colorM_B${config.colorM[2]}&"
        "colorS_R=${config.colorS[0]}&colorS_G=${config.colorS[1]}&colorS_B${config.colorS[2]}&"
        "poll_interval_min=${pollInterval}`;} function "
        "onValueChange(htmlElement,targetVar){switch(targetVar){case "
        "config.brightness:brightnessOutput.value=htmlElement.value;config.brightness=parseInt("
        "htmlElement.value);break;case "
        "config.showHours:config.showHours=!config.showHours;toggleHiddenById('colorH');break;case "
        "showMinutes:config.showMinutes=!config.showMinutes;toggleHiddenById('colorM');break;case "
        "config.showSeconds:config.showSeconds=!config.showSeconds;toggleHiddenById('colorS');"
        "break;case "
        "config.colorH:config.colorH=[parseInt(htmlElement.value.slice(1,3),16),parseInt("
        "htmlElement.value.slice(3,5),16),parseInt(htmlElement.value.slice(5,7),16),];break;case "
        "config.colorM:config.colorM=[parseInt(htmlElement.value.slice(1,3),16),parseInt("
        "htmlElement.value.slice(3,5),16),parseInt(htmlElement.value.slice(5,7),16),];break;case "
        "config.colorS:config.colorS=[parseInt(htmlElement.value.slice(1,3),16),parseInt("
        "htmlElement.value.slice(3,5),16),parseInt(htmlElement.value.slice(5,7),16),];break;case "
        "config.pollInterval:config.pollInterval=Math.min(Math.max(htmlElement.value,0),10000);"
        "break;default:break;}} function "
        "updateUIElements(){brightenessEl.value=config.brighteness;brightnessOutput.value=config."
        "brightness;showHoursEl.checked=config.showHours;showMinutesEl.checked=config.showMinutes;"
        "showSecondsEl.checked=config.showSeconds;hourColorEl.value='#'+get2DigitHex(config.colorH["
        "0])+get2DigitHex(config.colorH[1])+get2DigitHex(config.colorH[2]);minuteColorEl.value='#'+"
        "get2DigitHex(config.colorM[0])+get2DigitHex(config.colorM[1])+get2DigitHex(config.colorM["
        "2]);secondColorEl.value='#'+get2DigitHex(config.colorS[0])+get2DigitHex(config.colorS[1])+"
        "get2DigitHex(config.colorS[2]);pollingEl.value=config.pollInterval;} function "
        "get2DigitHex(value){return('00'+value.toString(16).toUpperCase()).slice(-2);}</script></"
        "head><body onload=onload()><div "
        "style=margin:auto;width:fit-content;padding:2rem;background-color:#efefef;height:100%><h1 "
        "style=border-radius:1rem;margin:auto;width:fit-content;padding:2rem;background:#000;color:"
        "#fff;margin-bottom:1rem>Verdandi Configuration Page</h1><div><table class=\"config-table "
        "width100\"><tbody><tr><td>Brightness:</td><td colspan=3><input id=brightnessSlider "
        "type=range value=50 max=100 min=0 oninput=onValueChange(this,config.brightness) "
        "step=5>&nbsp;<output "
        "id=brightnessOutput></output></td></tr><tr><td>Colors:</td><td><input id=showHours "
        "type=checkbox onchange=onValueChange(this,config.showHours) checked "
        "class=css-checkbox><label class=css-label for=colorH>&nbsp;H&nbsp;</label><input "
        "id=colorH type=color onchange=onValueChange(this,config.colorH) "
        "value=#ff0000></td><td><input id=showMinutes type=checkbox "
        "onchange=onValueChange(this,config.showMinutes) checked class=css-checkbox><label "
        "class=css-label for=colorM>&nbsp;M&nbsp;</label><input id=colorM type=color "
        "onchange=onValueChange(this,config.colorM) value=#00ff00></td><td><input id=showSeconds "
        "type=checkbox onchange=onValueChange(this,config.showSeconds) checked "
        "class=css-checkbox><label class=css-label for=colorS>&nbsp;S&nbsp;</label><input "
        "id=colorS type=color onchange=onValueChange(this,config.colorS) "
        "value=#0000ff></td></tr><tr><td>Poll interval:</td><td colspan=3><input id=pollInterval "
        "type=number onchange=onValueChange(this,config.pollInterval) value=10 max=600 min=1> "
        "min</td></tr></tbody></table><button "
        "onclick=sendUpdatedData()>Update</button></div></div></body></html>");
}

} // namespace Website
