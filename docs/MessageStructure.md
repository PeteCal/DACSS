For purposes of this project I use the following terminology:

* One bit is a "bit",
* Four bits form a "nibble",
* Eight bits (two nibbles) form a "byte".
* Eight bytes form a "WORD" (I use uppercase to distinguish my WORDs from common words).
* A group of WORDs form a sentence.
  

The Hex codes are in the Codes2device.xls file.  An explanation is below.

## *** Door Sensors (plug type)

The basic element for a door sensor seems to be a WORD that looks like:

FF7Fddddddaacccc
where:

"FF7F" is some sync stuff.

"dddddd" is a device code that is unique for each sensor.

"aa" is a 1 byte code that can indicate:

    * Opening, transmitted when that door opens.  ,    
    * Closing, transmitted when that door closes,    
    * StillOpen, transmitted about every 70 minutes to indicate it is alive and the door is still closed.    
    * StillClosed, transmitted about every 70 minutes to indicate it is alive and the door is still open.

"cccc" is a Cycle Redundancy Check.

I am sure there are WORDs for low battery but I haven't had that happen so I don't know what they might be.   Before I started this project the panel reported a low battery from one of the sensors and I replaced the battery.  I tried to create a low battery condition by replacing a battery with a variable power supply but as I lowered the voltage the device went dead before any "low battery" WORD was transmitted.  Perhaps the low battery conditions needs to be more gradual or must be present for one of the "Still" WORDs to be transmitted.  So now I wait for one of the batteries to go down. 

Also, There seems to be two "sentences".  When a device detects a change.  A sentence of 12 identical WORDs is sent.  First 6 WORDs then about a 1 second pause then 6 more.

When a Still message is sent the sentence is the same WORD sent just 6 times.

## *** Flat Door and Window Sensors

The Window sensors are similar except they have a "Tamper" switch that indicates that the electronics part has been unsnapped from the mounting.  So there are additional WORDs.  It seems that if the window is in a certain state (Open or Closed) and a Tamper occurs a WORD is transmitted with an additional bit set in the "aa" field.  Similarly, when the Tamper state is removed a similar WORD is sent with the Tamper bit cleared.

My flat door sensor seems to be unusual.  It doesn't have a unique WORD for StillOpen or StillClosed.  It just sends the same WORD as for Opening or Closing.  Tamper is added or removed if appropriate.  

Occasionally I get a WORD that is identical to a known WORD except the "cccc" is different.  I thought the CRC would be unique to a message and not different for the same bytes.

The "sentence" structure seems to be the same.

## *** Smoke and Heat Detectors

Each S&H detector has it's own "dddddd" and unique WORDs for Alive (==Still), Smoke detected, or Heat Detected.  But if I hit the "Test" button they both send the same Word followed by the unique Alive WORD so I can tell which one is being tested.

Again the "sentence" structure seems to be the same.  Alive is equivalent to Still.

## *** Glass Break Detector

I haven't gotten far with the Glass Break Detector.  It sends Alive.  To test it I need to create the sound of breaking glass.  I can purchase a simulator but I don't feel like putting up the cash.  I tried playing a glass break ringtone from my phone but I couldn't get it to trigger.  I did pick up a few WORDs by removing and installing the battery.  More work is needed here.

"The "sentence" structure is similar to the others.

## *** Med Panel.

I don't have a need for the Med Panel now.  I did some experimenting and it seems the buttons have some sort of rolling code.  More work is needed here.



