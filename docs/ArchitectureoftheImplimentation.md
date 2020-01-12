### Architecture of the Implementation.

This implementation uses only a few pieces of open source programs.  OpenHAB, mqtt mosquitto, and rtl_sdr .
I originally planned to use GNU Radio but when I tried to run it on my RPi it kept throwing segment faults.  So then I tried SoapySDR, PothosSDR, liquid-dsp, and probably a few others.  I found I just couldn't get them to do what I needed.  I estimate I spent over 100 hours grinding myself into the ground.  I even had some very knowledgeable advice from a few people but I couldn't make all the right things happen.  So I decided to do everything myself.

The program 345MHz sends a command to the RTL-SDR dongle, 

    rtl_sdr -f345000000 -s250000 -g49.0 /dev/shm/stream
    
which tunes it to 345MHz, sampling at 250k/s and writing to the file /dev/shm/stream.  I hope this file is in RAM and not on the SD card.  I don't want to beat the tar out of my SD card.

**bytes2mag.c** does the filtering.  I designed a 61 tap FIR filter and read the tap weights from TapsFIR.flt at startup.  bytes2mag.c then applies the filter to the contents of stream continuously and creates the filtered "mag" samples.

**mag2word.c** then takes the mag samples and processes them to synchronize with the manchester data stream.  It forms the WORD (see message structure) sent by the sensors and detectors. 

**word2sensor.c** attempts to associate the WORD with a sensor (or detector) by searching the file WORD2Action.csv.

**publish.c** will publish the device and action as designated by WORD2Action.csv