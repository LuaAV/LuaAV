/*
 *  Copyright 2009 Reality Jockey, Ltd.
 *                 info@rjdj.me
 *                 http://rjdj.me/
 * 
 *  This file is part of ZenGarden.
 *
 *  ZenGarden is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ZenGarden is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ZenGarden.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

package me.rjdj.zengarden;

import java.io.File;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.TargetDataLine;

/**
 * <code>ExampleGarden</code> is a a simple test class used to quickly test <code>ZenGarden</code>.
 * It contains a <code>main()</code> method and is not instantiable. Invoke the <code>main()</code>
 * method with:<br>
 * <pre>java -jar ZenGarden.jar path/to/zengarden/pd-patches/simple-osc.pd</pre>
 * Some patches may require microphone input. Make sure that your computer has one attached.
 * 
 * @author Martin Roth (mhroth@rjdj.me)
 */
public class ExampleGarden {
  
  // A huge block size is selected because the the Java Sound API is very slow on most platforms.
  private static final int BLOCK_SIZE = 2048;
  
  // 22050 Hz is chosen (somewhat arbitrarily) because this is the sample rate that RjDj runs at.
  private static final int SAMPLE_RATE = 44100;
  private static final int NUM_INPUT_CHANNELS = 2;
  private static final int NUM_OUTPUT_CHANNELS = 2;
  
  private static volatile boolean shouldContinuePlaying = true;
  
  private ExampleGarden() {
    // no instances of ExampleGarden allowed
  }
  
  public static void main(String[] args) {
    if (args.length == 0) {
      System.out.println("java ExampleGarden testpatch.pd");
    } else {
      // install a shutdown hook so that JVM shutdown (e.g., via CTRL-C) can be reacted to
      Runtime.getRuntime().addShutdownHook(new Thread() {
        @Override
        public void run() {
          shouldContinuePlaying = false;
        }
      });
      
      // configure the audio input and output lines
      AudioFormat inputAudioFormat = new AudioFormat(
          SAMPLE_RATE, 16, NUM_OUTPUT_CHANNELS, true, true);
      AudioFormat outputAudioFormat = new AudioFormat(
          SAMPLE_RATE, 16, NUM_INPUT_CHANNELS, true, true);
      DataLine.Info inputLineInfo = new DataLine.Info(TargetDataLine.class, inputAudioFormat);
      DataLine.Info outputLineInfo = new DataLine.Info(SourceDataLine.class, outputAudioFormat);
      
      short[] inputBuffer = new short[BLOCK_SIZE * NUM_INPUT_CHANNELS];
      short[] outputBuffer = new short[BLOCK_SIZE * NUM_OUTPUT_CHANNELS];
      byte[] bInputBuffer = new byte[inputBuffer.length * 2]; // 2 bytes per sample
      byte[] bOutputBuffer = new byte[outputBuffer.length * 2];
   
      TargetDataLine targetDataLine = null;
      SourceDataLine sourceDataLine = null;
      try {
        // TODO(mhroth): ensure that stereo input and output lines are actually being returned
        // by the system.
        
        // open the audio input (line-in or microphone)
        targetDataLine = (TargetDataLine) AudioSystem.getLine(inputLineInfo);
        targetDataLine.open(inputAudioFormat, bInputBuffer.length);
        targetDataLine.start();
        
        // open the audio output
        sourceDataLine = (SourceDataLine) AudioSystem.getLine(outputLineInfo);
        sourceDataLine.open(outputAudioFormat, bOutputBuffer.length);
        sourceDataLine.start();
      } catch (LineUnavailableException lue) {
        lue.printStackTrace(System.err);
        System.exit(1);
      }
      
      // load the Pd patch
      File pdFile = new File(args[0]);
      ZenGarden pdPatch = null;
      ZenGardenAdapter zgAdapter = new ZenGardenAdapter();
      try {
        pdPatch = new ZenGarden(pdFile, BLOCK_SIZE, NUM_INPUT_CHANNELS, NUM_OUTPUT_CHANNELS,
            (float) SAMPLE_RATE);
        pdPatch.addListener(zgAdapter);
      } catch (NativeLoadException nle) {
        nle.printStackTrace(System.err);
        System.exit(2);
      }
      
      while (shouldContinuePlaying) {
        // run the patch in an infinite loop
        targetDataLine.read(bInputBuffer, 0, bInputBuffer.length); // read from the input
        // convert the byte buffer to a short buffer
        for (int i = 0, j = 0; i < inputBuffer.length; i++) {
          inputBuffer[i] = (short) (((int) bInputBuffer[j++]) << 8);
          inputBuffer[i] |= ((short) bInputBuffer[j++]) & 0x00FF;
        }
        
        pdPatch.process(inputBuffer, outputBuffer);
        
        // convert short buffer to byte buffer
        for (int i = 0, j = 0; i < outputBuffer.length; i++) {
          bOutputBuffer[j++] = (byte) ((outputBuffer[i] & 0xFF00) >> 8);
          bOutputBuffer[j++] = (byte) (outputBuffer[i] & 0x00FF);
        }
        // write to the output
        sourceDataLine.write(bOutputBuffer, 0, bOutputBuffer.length);
      }
      
      // release all audio resources
      targetDataLine.drain();
      targetDataLine.close();
      sourceDataLine.drain();
      sourceDataLine.close();
    }
  }

}
