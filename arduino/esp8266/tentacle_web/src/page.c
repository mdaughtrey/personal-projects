// padding top right bottom left
const char * page =
"<!DOCTYPE html><html><head>"
"<style type=\"text/css\" id=\"style\">"
"input[type=\"color\"].custom {"
"padding: 0; border: none; height: 120px;"
"width: 280px; vertical-align: middle; }"
"button { width: 440px; border-radius: 5; padding: 20px 20px 20px 20px;"
"background-color: red; color: white; border-radius: 20px;"
"font-size: 72pt; }"
"ul { list-style-type: none }"
".dropbtn { background-color: green; color: white; padding: 20px 20px 20px 20px;"
"font-size: 72pt; border: none; -webkit-border-radius: 20px;"
"-moz-border-radius: 20px; border-radius: 20px; width: 440px;"
"margin: 2px 0px 2px 0px; }"
".dropdown { position: relative; display: inline-block; }"
".dropdown-content { display: none; position: absolute; background-color: blue;"
"width: 440px; border-radius: 20px; font-size: 72pt; }"
".dropdown-content a { font-family: sans-serif; background-color: blue;"
"font-size: 72pt; padding: 20px 20px 20px 20px;"
"text-decoration: none; display: block;"
"border-radius: 10px; color: white; }"
".colorcontainer { border-radius: 20px; background-color: blue;"
"font-family: sans-serif;"
"font-size: 72pt; padding: 20px 20px 20px 20px;"
"color: white; position: relative;"
"margin: 2px 0px 2px 0px; }"
".colorcontainer table tr td { padding: 20px 20px 20px 20px; }"
".dropdown-content a:hover { color: yellow; }"
".dropdown:hover .dropdown-content { display: block }"
"</style>"
"<script>"
"var serverIP = \"192.168.4.1\";"
"function onDocumentReady()"
"{"
"    var req = new XMLHttpRequest();"
"    req.onreadystatechange = function()"
"    {"
"        if (4 == req.readyState && 200 == req.status)"
"        {"
"            setState(req.responseText);"
"        }"
"    };"
"    req.open(\"GET\", \"http://\" + serverIP + \"/getstate\");"
"    req.send();"
"}"
""
"function setState(stateInfo)"
"{"
"    console.log(\"stateInfo \") + stateInfo;"
"    state = JSON.parse(stateInfo);"
"    if (state.hasOwnProperty(\"fight\"))"
"    {"
"        document.getElementById(\"fight\").innerHTML ="
"            state.fight ? \"HEY, RELAX\" : \"FIGHT\";"
"    }"
"}"
"function effect(id)"
"{"
"    var req = new XMLHttpRequest();"
"    req.open(\"GET\", \"http://\" + serverIP + \"/effect?id=\" + id, true);"
"    req.send();"
"    document.getElementById(\"dropbtn\").innerHTML ="
"        document.getElementById(id).innerHTML;"
"}"
"function fight()"
"{"
"    var req = new XMLHttpRequest();"
"    req.open(\"GET\", \"http://\" + serverIP + \"/fight\", true);"
"    req.onreadystatechange = function()"
"    {"
"        if (4 == req.readyState && 200 == req.status)"
"        {"
"            setState(req.responseText);"
"        }"
"    };"
"    req.send();"
"}"
"function ddhover(state)"
"{"
"    objs = document.getElementsByClassName(\"colorcontainer\");"
"    for (ii = 0; ii < objs.length; ii++)"
"    {"
"        objs[ii].style.display = state;"
"    }"
"    document.getElementById(\"fight\").style.display = state;"
"}"
"function bgcolor()"
"{"
"    color = document.getElementById(\"bgcolor\").value;"
"    var req = new XMLHttpRequest();"
"    req.open(\"GET\", \"http://\" + serverIP + \"/bg?color=\" + color.slice(1));"
"    req.send();"
"}"
"function fgcolor()"
"{"
"    color = document.getElementById(\"fgcolor\").value;"
"    var req = new XMLHttpRequest();"
"    req.open(\"GET\", \"http://\" + serverIP + \"/fg?color=\" + color.slice(1));"
"    req.send();"
"}"
"</script>"
"<title>Tentacle</title>"
"</head>"
"<body onload=\"onDocumentReady()\">"
"<ul>"
"<li><div class=\"dropdown\" onmouseover=\"ddhover('none')\" onmouseout=\"ddhover('block')\">"
"    <button id=\"dropbtn\" class=\"dropbtn\">Off</button>"
"    <div class=\"dropdown-content\">"
"        <a href=\"#\" id=\"off\" onclick=\"effect('off')\">Off</a>"
"        <a href=\"#\" id=\"all\" onclick=\"effect('all')\">All</a>"
"        <a href=\"#\" id=\"chase\" onclick=\"effect('chase')\">Chase</a>"
"        <a href=\"#\" id=\"trail\" onclick=\"effect('trail')\">Trail</a>"
"        <a href=\"#\" id=\"flash\" onclick=\"effect('flash')\">Flash</a>"
"        <a href=\"#\" id=\"heartbeat\" onclick=\"effect('heartbeat')\">Heartbeat</a>"
"    </div>"
"</div></li>"
""
"<li><div class=\"colorcontainer\"><table><tr>"
"        <td>Foreground</td>"
"        <td><input class=\"custom\" type=\"color\" id=\"fgcolor\" onchange=\"fgcolor()\"/></td>"
"</tr></table></div></li>"
""
"<li><div class=\"colorcontainer\"><table><tr>"
"        <td>Background</td>"
"        <td><input class=\"custom\" type=\"color\" id=\"bgcolor\" onchange=\"bgcolor()\"/></td>"
"</tr></table></div></li>"
"<li>"
"<button id=\"fight\" onclick=\"fight()\">FIGHT</button>"
"</li>"
"</ul>"
"</body>"
"</script>"
"</html>";
