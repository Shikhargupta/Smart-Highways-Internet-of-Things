# Smart-Highways-Internet-of-Things

This is the implementation of a robust system for real time monitoring of highways. Various sensors have been deployed on highway which report the status of the same to a central control station.

<p align="center">
<img src="images/Internet-of-things.jpg" width="500"/>
</p>

The highway monitoring system is divided into different modules:
* **Smart fencing**
* **Sound signal based accident detection**
* **Weather detection**
* **RFID toll tax collection**
* **Activity based road light**
* **Collision avoidance system**

## Hardware

The aforementioned modules were integrated using [Intel Galileo Gen 2](https://software.intel.com/en-us/iot/hardware/galileo) IoT kit. Arduino IDE was used to write sequential codes for the modules. 

<p align="center">
<img src="images/16106796_10209857079115973_925678796_o.png" width="500"/>
</p>

All the sensors used [ZigBee](https://www.digi.com/lp/xbee) protocol for wireless serial communication. MATLAB was used to detect accident sounds from the speech signals obtained.

