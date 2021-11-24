Commands = {
--    {"-", "Clear param", function() param = 0 end },
--    {"a", "Save SSID/PSK to store", function() creds[WiFi.SSID().c_str()] = WiFi.psk().c_str(); serialize();})},
--    {"A", "Load SSIDS from store", function() deserialize(); })},
--    {"C", "Clear store", function() EEPROM.write(0, 0); EEPROM.write(1, 0); EEPROM.commit();})},
--    {"d", "LED on", function() digitalWrite(2, LOW); })},
--    {"D", "LED off", function() digitalWrite(2, HIGH); })},
--    {"e", "Set FX Speed", function() fx.setSpeed(param); })},
--    {"f", "Set FX Mode", function() fx.setMode(param); })},
--    {"h", "Help", function() help(); listStoredNetworks(); })},jjj
h = {help="Help", fun=function() help() end }
--    {"m", "Connect to MQTT", function() mqttConnect(); })},
--    {"M", "Disconnect from MQTT", function() mqttClient->disconnect(); })},
--    {"n", "Scan and connect", function()  connect(); })},
--    {"r", "Reset", function()  verbose("Resetting...\r\n"); while(1); })},
--    {"w", "Webserver on", function()  initWebServer(); webactive = true; })},
--    {"W", "Webserver off", function()  webactive = false; })}
}

function help()
--    print("Help!\r\n")
    for kk,vv in pairs(Commands)
    do
        print(string.format("%s: %s", kk, vv["help"]))
    end
end

function handleCommand(data)
--    uart.write(0, string.format("Recieved data %s\r\n", data))
    key = string.sub(data, 1, 1)
    if Commands[key]
    then
        Commands[key]["fun"]()
    end
end -- function

function init()
    uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1, 1)
    uart.on("data", 0, handleCommand, 0)
end


function main()
--    init()
--    uart.write(0, "Hello\r\n")
end

    
handleCommand("h")