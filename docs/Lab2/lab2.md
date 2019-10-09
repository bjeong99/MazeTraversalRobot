# Lab 2
## Monday 09/16/2019

In this lab we implemented a Fast-Fourier-Transform (FFT) to identify a specific tone that is to eventually be generated as a start tone at competition. This was done by using the Arduino FFT Library, polling an analog audio sensor, along with multiple filters, both digital and analog.

To get the microphone working, we first started with the stock schematic that was provided to us on the lab 2 website. This schematic is shown below. With the microphone wired to analog pin A0, we were then able to use the FFT library from Arduino to get the Fast-Fourier Transform of the audio. This code is parsed below.

```cpp
ADCSRA = 0xE5; //Turns the ADC on in free running mode (i.e latches only when we tell it to take a sample)
ADMUX  = 0x40; //Sets the source to ADC0 (hard wired to pin A0)
DIDR0  = 0x01; //Turns off the digital input (on by default) on pin A0
```

It is worth taking a look at the ADC status register, ADCSRA, and noting that the prescale bits, ADCSRA[0:2], are being set to 3b'101. This sets the clock division factor of the ADC at 32, meaning that the ADC internal clock will run at 16Mhz /32 = 500Khz. This paired with the 13 cycles that the successive approximation ADC takes to convert a value means that the maximum polling frequency is actually at 500Mhz / 13 = 34.5Khz or 26 microseconds between sample. This will become important later on when we are trying to decifer the output of the FFT.

We then enter our next code block from the example code, which uses a for loop to take 256 samples in sucession to generate an array suitable for an FFT. This is done by waiting for the ADC to finish the conversion, renabling the ADC, and while we are waiting for the ADC to finish polling we add the element to the fft_input array.

```cpp
for (int i = 0; i < 512; i += 2) {
   while(!(ADCSRA & 0x10)); // wait for adc to be ready
   ADCSRA = 0xf5; // restart adc
   byte m = ADCL; // fetch adc data
   byte j = ADCH;
   int k = (j << 8) | m; // form into an int
   k -= 0x0200; // form into a signed int
   k <<= 6; // form into a 16b signed int
   fft_input[i] = k; // put real data into even bins
   fft_input[i+1] = 0; // set odd bins to 0
}
```

Since we start the ADC conversion at the beggining of the while loop, we can use precious CPU time to get the previous sample value and put it into the fft_input array before the next sample is done. This allows us to keep our sample rate extremely close to 34.5Khz. This sample rate is important in determining where exactly the 950Hz frequency is.

The output of the FFT function is an array called fft_log_out[i], this is an array of length 128 (since we have 256 samples). To find the frequency of importance, 950Hz, we have to calculate the closet "bin" to it. Since this is a fast fourier transform, each of the frequencies get lumped into #samples/2 groups whose harmonic frequency can be found by the formula f = n * (Sample Rate / # of Samples). If we solve for n with Sample Rate = 34.5Khz, Samples = 256, and f = 950Hz, we get that n = 6.88. This means that we are going to be looking for a spike in bin 7, as this is where the majority of the magnitude from 950Hz will show up. Below is the section of code that we used to identify if there was a 950Hz tone being played, and if so, make the LED tied to pin 13 light up.

```cpp
if (fft_log_out[7] - fft_log_out[6] > 20 && fft_log_out[7] - fft_log_out[8] > 20) {
  digitalWrite(13, HIGH);
}
```

We ended up using a double comparison for the fft magnitude so as to rule out outside noise that may or may not be present. The threshold of 20dB was decided through testing the microphone with different tones and seeing how the fft plot reacted. Below are some of the serial plots that we recored while doing this lab.

<p align="center">
   <img src="no950.png" height="60%" width="60%">
   <br>
   <a>No tone being played</a>
</p>

<p align="center">
   <img src="yes950.png" height="60%" width="60%">
   <br>
   <a>Tone being played</a>
</p>

As you can see in the second image of the FFT, there is a very distinct peak right after the harmonic frequency. This is the spike for 950Hz being played. It appears so close to the fundemental because n = 7 where n can range from 0 to 128.

***3. Filtering the incoming signal***

