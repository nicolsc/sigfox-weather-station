#Weather station with SmartEverything

##What

Get sensors values, and send them through the SIGFOX Network
* Pressure in _mbar_
* Temperature in _°C_
* Humidity in _%_

Uses the [http://smarteverything.it](SmartEverything) Arduino board, with a SIGFOX module included & various sensors

##View data on the SIGFOX Cloud

* Get to your device page (https://backend.sigfox.com/device/:deviceid/info)
* Click on the _device type_ name
* Click _Edit_
* Update the _Display type_
	* Custom
	* `pressure::uint:32 temperature::float:32 humidity::uint:32`

* Enjoy :)


Now, you should see the decoded frame in the _Device messages_ view
