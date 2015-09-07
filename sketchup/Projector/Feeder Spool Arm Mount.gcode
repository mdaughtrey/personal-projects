M109 S208.000000
;Sliced at: Mon 17-08-2015 23:37:55
;Basic settings: Layer height: 0.2 Walls: 0.8 Fill: 20
;Print time: 0 minutes
;Filament used: 0.0m 0.0g
;Filament cost: None
;M190 S70 ;Uncomment to add your own bed temperature line
;M109 S208 ;Uncomment to add your own temperature line
G21        ;metric values
G90        ;absolute positioning
M82        ;set extruder to absolute mode
M107       ;start with the fan off
G28 X0 Y0  ;move X/Y to min endstops
G28 Z0     ;move Z to min endstops
G29        ;Run the auto bed leveling
G1 Z15.0 F4200 ;move the platform down 15mm
G92 E0                  ;zero the extruded length
G1 F200 E3              ;extrude 3mm of feed stock
G92 E0                  ;zero the extruded length again
G1 F4200
;Put printing message on LCD screen
M117 Printing...

;Layer count: 10
;LAYER:0
M107
;LAYER:1
M106 S255
;LAYER:2
;LAYER:3
;LAYER:4
;LAYER:5
;LAYER:6
;LAYER:7
;LAYER:8
;LAYER:9
M107
G1 F1800 E-1.00000
G0 F4200 X0.000 Y0.000 Z7.000
;End GCode
M104 S0                     ;extruder heater off
M140 S0                     ;heated bed heater off (if you have it)
G91                                    ;relative positioning
G1 E-1 F300                            ;retract the filament a bit before lifting the nozzle, to release some of the pressure
G1 Z+0.5 E-5 X-20 Y-20 F4200 ;move Z up a bit and retract filament even more
G28 X0 Y0                              ;move X/Y to min endstops, so the head is out of the way
M84                         ;steppers off
G90                         ;absolute positioning
;CURA_PROFILE_STRING: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

