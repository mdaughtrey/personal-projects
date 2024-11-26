---
title: 'A Mask For Every Occasion'
date: '2020-10-20T23:06:06-04:00'
author: mdaughtrey
layout: post
categories:
    - Projects
    - BlueMouth
---

|![](/assets/uploads/2020/10/underbite-150x150.jpg)|![](/assets/uploads/2020/10/snarl-150x150.jpg)|![](/assets/uploads/2020/10/smile-150x150.jpg)|
|![](/assets/uploads/2020/10/overbite-150x150.jpg)|![](/assets/uploads/2020/10/hackaday-150x150.jpg)|![](/assets/uploads/2020/10/gritted-150x150.jpg)|

</figure>## A mask-mounted e-paper display, controlled from your phone.


A fairly simple design done in [OpenSCAD](http://www.openscad.org/). It’s two parts; The cyan clip fits over the yellow assembly. The parts are printed in PLA so I glued the two parts together with dichloromethane solvent (available as [Scigrip 16](https://www.amazon.com/s?k=scigrip+16&crid=1GZGEW2J5Q9K3&sprefix=scigrip%2Caps%2C133&ref=nb_sb_ss_i_2_7) at Amazon) and then cut the mask fabric out. The design file is [here](https://github.com/mdaughtrey/openscad/blob/master/bluemouth/bluemouth.scad)

![](/assets/uploads/2020/10/image-300x217.png)
![](/assets/uploads/2020/10/snarl.jpg)

## Display Installation

Here is the mounted e-paper display with an adapter board also acting as strain relief:.

![](/assets/uploads/2020/10/mounted-1-233x300.jpg)
![](/assets/uploads/2020/10/smile.jpg)## E-Paper Display and Driver

The e-paper display is a 2.9inch flexible display from [Waveshare ](https://www.waveshare.com/product/displays/e-paper/epaper-2/2.9inch-e-paper-d.htm)and [Aliexpress](https://www.aliexpress.com/item/32922604788.html?spm=a2g0o.productlist.0.0.798f72759DOwkb&algo_pvid=0bf8d789-ec06-4d99-8209-729a705f2530&algo_expid=0bf8d789-ec06-4d99-8209-729a705f2530-0&btsid=0bb0624216031648794658905e68db&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)

That connects to an ESP32 E-Paper driver board ([Aliexpress](https://www.aliexpress.com/item/4000804064988.html?spm=a2g0s.9042311.0.0.148b4c4dXyq1Qj)). It’s an ESP32-WROOM-32 module with connector hardware.

![](/assets/uploads/2020/10/esp32-300x283.jpg)
![](/assets/uploads/2020/10/overbite.jpg)
## Image Preparation

The e-paper display is 296 x 128. I couldn’t find any mouth images I liked on the net and I can’t draw mouths, so I took a picture of my own face, cropped my mouth and drew around it, then saved the result as a 1 bit BMP. I’ll spare you the original image, but here’s a processed example:

![](/assets/uploads/2020/10/overbiteR.jpg)
![](/assets/uploads/2020/10/hackaday.jpg)

## ESP32 Software

The code is a mixture of the BLE client sample code that comes with the BLE library, the proof of concept code for the e-paper display, and my own logic. I would have like to use the [GxEPD2 ](https://github.com/ZinggJM/GxEPD2)library but it doesn’t yet support my display. Instead I’ve hacked up the [sample code from github](https://github.com/waveshare/e-Paper/tree/master/Arduino). The screen refresh is pretty rough still.

I used arduino-cli in a Docker container to develop the ESP32 code. The code is [here](https://github.com/mdaughtrey/personal-projects/tree/master/arduino/arduino-cli/eink/bluemouth/bluemouth)

![](/assets/uploads/2020/10/gritted.jpg)
## Phone Control Javascript

The ESP32 module runs as a Bluetooth Low Energy (BLE) client. This allows control from the phone. The phone app is written using [phonk.app](https://phonk.app/), which allows most of the phone functions to be accessed using Javascript. This system is a work in progress but it’s very useful already.  
The communication between the phone and the ESP32 is a simple serial command interface; the ESP responds to a set of simple commands and displays the selected image.

![](/assets/uploads/2020/10/phonk-2-146x300.png)
## The phone code:

{% raw %}
<pre class="wp-block-code">```
ui.addTitle('BlueMouth')

network.bluetoothLE.start()
var bleClient = network.bluetoothLE.createClient()

// -> Write here the BLE device MAC Address
// var deviceMacAddress = '24:6F:28:7A:5C:CE'
var deviceMacAddress = '4C:11:AE:79:8E:8A';
var serviceUUID = '6E400001-B5A3-F393-E0A9-E50E24DCCA9E'
var characteristicUUID_TX = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'
var characteristicUUID_RX = '6e400003-b5a3-f393-e0a9-e50e24dcca9e'

function setResources(data)
{
var list = ui.addList(0.1, 0.1, 0.85, 0.8).init(
  data,
  function () { // on create view`
    return ui.newView('button')
  },
  function (o) { // data binding
    var t = data[o.position]
    o.view.props.background = '#00FFFFFF'
    o.view.props.padding = 50
    o.view.text(t)
    o.view.onClick(function () {
      bleClient.write(t, deviceMacAddress, serviceUUID, characteristicUUID_TX)
    }
  )
  })}
  
network.bluetoothLE.start()
// scan bluetooth low energy networks
// this should show devices -> services -> characteristics
var btn = ui.addButton('Get', 0.1, 0.9, 0.4, 0.1).onClick(function () {
      bleClient.write("list", deviceMacAddress, serviceUUID, characteristicUUID_TX)
})

// connect to a device
ui.addToggle(['Connect', 'Disconnect'], 0.55, 0.9, 0.4, 0.1).onChange(function (e) {
  if (e.checked) {
    bleClient.connectDevice(deviceMacAddress)
  } else {
    bleClient.disconnectDevice(deviceMacAddress)
  }
})

bleClient.onNewDeviceStatus(function (e) {
  // double check if is the device we want to connect
  if (e.deviceMac === deviceMacAddress && e.status === 'connected') {
    bleClient.readFromCharacteristic(deviceMacAddress, serviceUUID, characteristicUUID_RX)
    //txt.add('connected to ' + e.deviceName)
  }
})

bleClient.onNewData(function (e) {
  console.log("newData");
  var value = util.parseBytes(e.value, 'string')
  elems = value.split(',')
  if ("list" == elems[0])
  {
    setResources(elems.slice(1,))
  }
})
{% endraw %}


