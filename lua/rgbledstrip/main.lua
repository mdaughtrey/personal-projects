configpage="<!DOCTYPE html><html><head><title>{{hostname}} Configuration</title></head><body> <h2>{{hostname}} Configuration</h2><form action=\"/setconfig\"><table><tr><td><label for=\"hname\">Hostname:</label></td><td><input type=\"text\" id=\"hname\" name=\"hname\" value=\"{{hostname}}\"><br></td></tr><tr><td><label for=\"sname\">MQTT Server:</label></td><td><input type=\"text\" id=\"sname\" name=\"sname\" value=\"{{mqtt_server}}\"><br></td></tr><tr><td><label for=\"mport\">MQTT Port:</label></td><td><input type=\"numeric\" id=\"mport\" name=\"mport\" value=\"{{mqtt_port}}\"><br></td></tr></table><input type=\"submit\" value=\"Submit\"></form><p>The device will restart on submit.</p></body></html>";
webactive = false
param = 0
hostname = "colorbar"
mqtt_server = "server"
mqtt_port = 1883

WiFi = {hostname = "colorbar", SSID = "thessid", psk = "thepsk"}

fx = {
    _mode = 0,
    _speed = 0,
    _red = 0,
    _green = 0,
    _blue = 0,
    setMode = function(m) _mode = m end,
    setSpeed = function(s) _speed = s end
}

Commands = {
["-"] = {help="Clear param", fun=function() param = 0 end },
a = {help="Save SSID/PSK to store", fun=function() storedNetworks[WiFi.SSID] = WiFi.psk serialize() end},
A = {help="Load values  from store", fun=function() deserialize() end },
--    {"C", "Clear store", function() EEPROM.write(0, 0); EEPROM.write(1, 0); EEPROM.commit();})},
--    {"d", "LED on", function() digitalWrite(2, LOW); })},
--    {"D", "LED off", function() digitalWrite(2, HIGH); })},
e = {help="Set FX Speed", fun=function() fx.setSpeed(param) end },
f = {help="Set FX Mode", fun=function() fx.setMode(param) end },
h = {help="Help", fun=function() help() end },
m = {help="Connect to MQTT", fun=function() mqttConnect() end },
--    {"M", "Disconnect from MQTT", function() mqttClient->disconnect(); })},
n = {help="Scan and connect", fun=function()  connect() end },
r = {help="Reset", fun=function()  verbose("Resetting...\r\n") end },
w = {help="Webserver on", fun=function()  initWebServer() webactive = true end },
W = {help="Webserver off", fun=function()  webactive = false end }
}

topics = {
    {subtopic="led", subfun = function() print("led subfun") end,
         pubtopic="ledstatus", pubfun=function() print("ledstatus pubfun") end},
    {subtopic="fx", subfun = function() print("fx subfun") end,
         pubtopic="fxstatus", pubfun=function() print("fxstatus pubfun") end},
    {subtopic="speed", subfun = function() print("speed subfun") end,
         pubtopic="speedstatus", pubfun=function() print("speedstatus pubfun") end},
    {subtopic="bright", subfun = function() print("bright subfun") end,
         pubtopic="brightstatus", pubfun=function() print("brightstatus pubfun") end},
    {subtopic="bright", subfun = function() print("bright subfun") end,
         pubtopic="brightstatus", pubfun=function() print("brightstatus pubfun") end},
    {subtopic="color", subfun = function() print("color subfun") end,
         pubtopic="colorstatus", pubfun=function() print("colorstatus pubfun") end},
    {subtopic="fxnext", subfun = function() print("fxnext subfun") end,
         pubtopic="fxnextstatus", pubfun=function() print("fxnextstatus pubfun") end},
    {subtopic="fxprev", subfun = function() print("fxprev subfun") end,
         pubtopic="fxprevstatus", pubfun=function() print("fxprevstatus pubfun") end},
    {subtopic="red", subfun = function() print("red subfun") end,
         pubtopic="redstatus", pubfun=function() print("redstatus pubfun") end},
    {subtopic="green", subfun = function() print("green subfun") end,
         pubtopic="greenstatus", pubfun=function() print("greenstatus pubfun") end},
    {subtopic="blue", subfun = function() print("blue subfun") end,
         pubtopic="bluestatus", pubfun=function() print("bluestatus pubfun") end}
}

storedNetworks = {
    wifi0 = "psk0",
    wifi1 = "psk1"
}

function pageFixup(html)
    html = string.gsub(html, "{{hostname}}", WiFi.hostname)
    html = string.gsub(html, "{{mqtt_server}}", mqtt_server)
    html = string.gsub(html, "{{mqtt_port}}", mqtt_port)
    return html;
end

function buildTopics()
    print("buildTopics")
    for _,vv in pairs(topics)
    do
        vv["subtopic"] = string.format("/%s/%s", hostname, vv["subtopic"])
        print(string.format("Topic %s", vv["subtopic"]))
        
    end
end

function init()
    param = 0
end

function deserialize()
    print("deserialize")
end

function mqttConnect()
    print("mqttConnect")
end

function connect()
    print("connect")
    buildTopics()
end

function initWebServer()
    print("initWebServer")
end

function help()
    for kk,vv in pairs(Commands)
    do
        print(string.format("%s: %s", kk, vv["help"]))
    end
    print("Stored Networks:")
    for ii,vv in ipairs(storedNetworks)
    do
        print(string.format("%u: %s", ii, vv[SSID]))
    end
end

function handleCommand(data)
--    uart.write(0, string.format("Recieved data %s\r\n", data))
    local key = string.sub(data, 1, 1)
    if Commands[key]
    then
        Commands[key]["fun"]()
    elseif type(key) == "number" and key >= 0 and key <= 9
    then
        param = param * 10 + key
    end
end -- function

function init()
    uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1, 1)
    uart.on("data", 0, handleCommand, 0)
end

function main()
--    init()
--    uart.write(0, "Hello\r\n")
    while(true)
    do
        io.flush()
        local input = io.read(1)
        print("Input is", input)
        handleCommand(input)
    end
end

main()