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
d = {help="LED on", fun=function() gpio.write(4, gpio.LOW) end },
D = {help="LED off", fun=function() gpio.write(4, gpio.HIGH) end},
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
--    {subtopic="led", subfun = function() print("led subfun") end,
--         pubtopic="ledstatus", pubfun=function() print("ledstatus pubfun") end},
--    {subtopic="fx", subfun = function() print("fx subfun") end,
--         pubtopic="fxstatus", pubfun=function() print("fxstatus pubfun") end},
--    {subtopic="speed", subfun = function() print("speed subfun") end,
--         pubtopic="speedstatus", pubfun=function() print("speedstatus pubfun") end},
--    {subtopic="bright", subfun = function() print("bright subfun") end,
--         pubtopic="brightstatus", pubfun=function() print("brightstatus pubfun") end},
--    {subtopic="bright", subfun = function() print("bright subfun") end,
--         pubtopic="brightstatus", pubfun=function() print("brightstatus pubfun") end},
--    {subtopic="color", subfun = function() print("color subfun") end,
--         pubtopic="colorstatus", pubfun=function() print("colorstatus pubfun") end},
--    {subtopic="fxnext", subfun = function() print("fxnext subfun") end,
--         pubtopic="fxnextstatus", pubfun=function() print("fxnextstatus pubfun") end},
--    {subtopic="fxprev", subfun = function() print("fxprev subfun") end,
--         pubtopic="fxprevstatus", pubfun=function() print("fxprevstatus pubfun") end},
--    {subtopic="red", subfun = function() print("red subfun") end,
--         pubtopic="redstatus", pubfun=function() print("redstatus pubfun") end},
--    {subtopic="green", subfun = function() print("green subfun") end,
--         pubtopic="greenstatus", pubfun=function() print("greenstatus pubfun") end},
--    {subtopic="blue", subfun = function() print("blue subfun") end,
--         pubtopic="bluestatus", pubfun=function() print("bluestatus pubfun") end}
}

config = {
    mqttServer = "localhost",
    mqttPort = 1883,
    hostname = "none",
    ssids = {
        {ssid="testssid0", psk="testpsk0"},
        {ssid="testssid1", psk="testpsk1"}
    }

}

function listAP(t)
    for k,v in pairs(t) do
        print(k..": "..v)
        if config["ssids"][k]
        then
            print("matched on "..config["ssids"][k])
            wifi.sta.config({ssid=config["ssids"][k]["ssid"],
            pwd==config["ssids"][k]["psk"],
            save=true})
            wifi.sta.connect(onWifiConnect)
        end
    end
    print("No known APs, connecting via WPS...")
    wps.enable()
    wps.start(onWPSEnd)
end

function onWiFiConnect()
    mqttConnect()
end

function onWPSEnd(status)
    print("onWPSEnd status "..status)
    if status == wps.SUCCESS then
        wps.disable()
        print("WPS: Success, connecting to AP...")
        wifi.sta.connect()
        return
    elseif status == wps.FAILED then
        print("WPS: Failed")
    elseif status == wps.TIMEOUT then
        print("WPS: Timeout")
    elseif status == wps.WEP then
        print("WPS: WEP not supported")
    elseif status == wps.SCAN_ERR then
        print("WPS: AP not found")
    else
        print(status)
    end
    wps.disable()
end



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

function deserialize()
    print("deserialize")
end

function mqttConnect()
    print("mqttConnect")
end

function connect()
    print("connect")
    wifi.sta.getap(listAP)
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
    uart.write(0, string.format("Recieved data %s\r\n", data))
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
    param = 0
    gpio.mode(4, gpio.OUTPUT)
    wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, function(T)
        print("\n\tSTA - CONNECTED".."\n\tSSID: "..T.SSID.."\n\tBSSID: "..
        T.BSSID.."\n\tChannel: "..T.channel)
    end)

    wifi.eventmon.register(wifi.eventmon.STA_DISCONNECTED, function(T)
        print("\n\tSTA - DISCONNECTED".."\n\tSSID: "..T.SSID.."\n\tBSSID: "..
        T.BSSID.."\n\treason: "..T.reason)
    end)

    wifi.eventmon.register(wifi.eventmon.STA_AUTHMODE_CHANGE, function(T)
        print("\n\tSTA - AUTHMODE CHANGE".."\n\told_auth_mode: "..
        T.old_auth_mode.."\n\tnew_auth_mode: "..T.new_auth_mode)
    end)

    wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, function(T)
        print("\n\tSTA - GOT IP".."\n\tStation IP: "..T.IP.."\n\tSubnet mask: "..
        T.netmask.."\n\tGateway IP: "..T.gateway)
    end)

    wifi.eventmon.register(wifi.eventmon.STA_DHCP_TIMEOUT, function()
        print("\n\tSTA - DHCP TIMEOUT")
    end)

    wifi.eventmon.register(wifi.eventmon.AP_STACONNECTED, function(T)
        print("\n\tAP - STATION CONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
    end)

    wifi.eventmon.register(wifi.eventmon.AP_STADISCONNECTED, function(T)
        print("\n\tAP - STATION DISCONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
    end)

    wifi.eventmon.register(wifi.eventmon.AP_PROBEREQRECVED, function(T)
        print("\n\tAP - PROBE REQUEST RECEIVED".."\n\tMAC: ".. T.MAC.."\n\tRSSI: "..T.RSSI)
    end)

    wifi.eventmon.register(wifi.eventmon.WIFI_MODE_CHANGED, function(T)
        print("\n\tSTA - WIFI MODE CHANGED".."\n\told_mode: "..
        T.old_mode.."\n\tnew_mode: "..T.new_mode)
    end)
--     wifi.autoconnect()
    uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1, 1)
    uart.on("data", 0, handleCommand, 0)
    uart.write(0, "Hello\r\n")
end

-- function main()
--     init()
--     while(true)
--     do
--         io.flush()
--         local input = io.read(1)
--         print("Input is", input)
--         handleCommand(input)
--     end
-- end

init()
-- main()
