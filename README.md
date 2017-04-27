#Weather station with SmartEverything

##What

Get sensors values, and send them through the SIGFOX Network
* Pressure in _mbar_
* Temperature in _°C_
* Humidity in _%_

Uses the [SmartEverything](http://smarteverything.it) Arduino board, with a SIGFOX module included & various sensors

##View data on the SIGFOX Cloud

* Get to your device page (https://backend.sigfox.com/device/:deviceid/info)
* Click on the _device type_ name
* Click _Edit_
* Update the _Display type_
	* Custom
	* `pressure::uint:16 temperature::float:32 humidity::uint:16`

* Enjoy :)


Now, you should see the decoded frame in the _Device messages_ view

##Thanks

* @aboudou, as i used his [SmartEverything GPS+Sigfox project](https://github.com/aboudou/SmartEverything_SigFox_GPS) as a boilerplate
