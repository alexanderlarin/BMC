/*
  See https://www.RoxXxtar.com/bmc for more details
  Copyright (c) 2020 RoxXxtar.com
  Licensed under the MIT license.
  See LICENSE file in the project root for full license information.
*/
#ifndef BMC_DAW_LC_H
#define BMC_DAW_LC_H

#include "utility/BMC-Def.h"
#include "midi/BMC-MidiClock.h"

#ifdef BMC_USE_DAW_LC

class BMCDawLogicControl {
private:
  BMCMidi& midi;
  bmcStoreGlobal& global;
  BMCCallbacks& callback;
  BMCLogicControlData controller;
public:
  BMCDawLogicControl(BMCMidi& t_midi, bmcStoreGlobal& t_global, BMCCallbacks& cb):
              midi(t_midi),
              global(t_global),
              callback(cb)
  {
    //
  }
  void update(){
    controller.update();
    for(uint8_t i=0;i<8;i++){
      if(controller.getMeterChanged(i) && callback.dawChannelMeterUpdate){
        uint8_t level = controller.getMeter(i);
        uint8_t overload = controller.getMeterOverload(i);
        callback.dawChannelMeterUpdate(i, level, overload);
      }
    }
  }
  uint8_t getSelectedChannel(){
    return controller.getSelectedChannel();
  }
  uint8_t getVPotLedState(uint8_t channel, uint8_t ledN){
    return controller.getVPotValue(channel, ledN);
  }
  bool incoming(BMCMidiMessage d){
    if(!d.matchSource(BMC_USB)){
      return false;
    }
    if(d.isSysEx()){
      if(d.sysex[1]==0 && d.sysex[2]==0 && d.sysex[3]==0x66 && d.sysex[4]==0x10){
        incomingSysEx(d);
        return true;
      }
    }
    if(!controller.isOnline()){
      return false;
    }
    uint8_t c = d.getData1();
    uint8_t v = d.getData2();
    if(d.isControlChange()){
      if(c>=0x30 && c<=0x37){ // v-select
        uint8_t ch = (c-0x30);
        uint8_t centered = bitRead(v, 6);
        uint8_t mode = (v >> 4) & 0x03;
        uint8_t value = v & 0x0F;
        controller.setVPot(c-0x30, v);
        if(callback.dawChannelVPotUpdate){
          callback.dawChannelVPotUpdate(ch, value, centered, mode);
        }
      } else if((c & 0xF0) == 0x40){
    		uint8_t  digit = c & 0x0F;
        BMC_PRINTLN(">",c,digit,v);
    		if(digit < 10 && callback.dawReceivedTimeCodeDigit){
          callback.dawReceivedTimeCodeDigit(digit, v);
    		}
    	}
    } else if(d.isAfterTouch()){
      if(d.getChannel()==1){
        uint8_t ch = (c>>4) & 0x07;
        uint8_t level = c & 0x0F;
        controller.setMeter(ch, level);
        if(callback.dawRecievedChannelMeter){
          callback.dawRecievedChannelMeter(ch, level);
        }
      }
    } else if(d.isNoteOn() || d.isNoteOff()){
      if(c<=0x07){ // rec/ready
        controller.setRecState(c, (v>0));
        if(callback.dawRecievedChannelRecReady){
          callback.dawRecievedChannelRecReady(c, (v>0));
        }
      } else if(c>=0x08 && c<=0x0F){ // solo
        controller.setSoloState((c-0x08), v>0);
        if(callback.dawRecievedChannelSolo){
          callback.dawRecievedChannelSolo(c-0x08, (v>0));
        }
      } else if(c>=0x10 && c<=0x17){ // mute
        controller.setMuteState((c-0x10), (v>0));
        if(callback.dawRecievedChannelMute){
          callback.dawRecievedChannelMute(c-0x10, (v>0));
        }
      } else if(c>=0x18 && c<=0x1F){ // select
        controller.setSelectState((c-0x18), (v>0));
        if(v==127){
          controller.setSelectedChannel(c-0x18);
          if(callback.dawChannelVPotUpdate){
            uint8_t ch = controller.getSelectedChannel();
            uint8_t centered = controller.getVPotCentered(ch);
            uint8_t mode = controller.getVPotMode(ch);
            uint8_t value = controller.getVPotValue(ch, 0);
            callback.dawChannelVPotUpdate(ch, value, centered, mode);
          }
        }
        if(callback.dawRecievedChannelSelect){
          callback.dawRecievedChannelSelect((c-0x18), (v>0));
        }
      } else {
        bool ret = false;
        switch(c){
          case 0x28: controller.setAssignTrack(v>0); ret = true; break;
          case 0x29: controller.setAssignSend(v>0); ret = true; break;
          case 0x2A: controller.setAssignPan(v>0); ret = true; break;
          case 0x2B: controller.setAssignPlugin(v>0); ret = true; break;
          case 0x2C: controller.setAssignEQ(v>0); ret = true; break;
          case 0x2D: controller.setAssignInstr(v>0); ret = true; break;
          case 0x32: controller.setFaderViewFlip(v>0); ret = true; break;
          case 0x33: controller.setFaderViewGlobal(v>0); ret = true; break;
          case 0x4A: controller.setAutomationRead(v>0); ret = true; break;
          case 0x4B: controller.setAutomationWrite(v>0); ret = true; break;
          case 0x4C: controller.setAutomationTrim(v>0); ret = true; break;
          case 0x4D: controller.setAutomationTouch(v>0); ret = true; break;
          case 0x4E: controller.setAutomationLatch(v>0); ret = true; break;
          case 0x4F: controller.setAutomationGroup(v>0); ret = true; break;
          case 0x50: controller.setUtilitySave(v>0); ret = true; break;
          case 0x51: controller.setUtilityUndo(v>0); ret = true; break;
          case 0x54: controller.setTransportMarker(v>0); ret = true; break;
          case 0x55: controller.setTransportNudge(v>0); ret = true; break;
          case 0x56: controller.setTransportCycle(v>0); ret = true; break;
          case 0x57: controller.setTransportDrop(v>0); ret = true; break;
          case 0x58: controller.setTransportReplace(v>0); ret = true; break;
          case 0x59: controller.setTransportClick(v>0); ret = true; break;
          case 0x5A: controller.setTransportSolo(v>0); ret = true; break;
          case 0x5B: controller.setTransportRewind(v>0); ret = true; break;
          case 0x5C: controller.setTransportForward(v>0); ret = true; break;
          case 0x5D: controller.setTransportStop(v>0); ret = true; break;
          case 0x5E: controller.setTransportPlay(v>0);; ret = true; break;
          case 0x5F: controller.setTransportRecord(v>0); ret = true; break;
          case 0x64: controller.setTransportCursorZoom(v>0); ret = true; break;
          case 0x65: controller.setTransportScrub(v>0); ret = true; break;
        }
        if(ret && callback.dawRecievedLedState){
          callback.dawRecievedLedState(c, v>0);
        }
      }
    } else if(d.isPitchBend()){
      uint8_t ch = d.getChannel()-1;
      int value = d.getPitchValue() & 0x3FFC;
      value = map(value, 0, 14843, (-8192), 6651);
      // 6651
      //-8192

      // in logic pro
      // fader at max/+6db = 14843
      // fader at unity/0db = 12441
      // fader at min/infinity = 0

      if(ch==8){
        controller.setMasterVolume(value);
        if(callback.dawRecievedMasterFaderPosition){
          callback.dawRecievedMasterFaderPosition(value);
        }
      } else {
        controller.setVolume(ch, value);
        if(callback.dawRecievedChannelFaderPosition){
          callback.dawRecievedChannelFaderPosition(ch, value);
        }
      }
    }
    return false;
  }
  void incomingSysEx(BMCMidiMessage d){
    switch(d.sysex[5]){
      case 0x00:
        sendHostConnectionQuery();
        break;
      case 0x02:
        controller.setOnline();
        if(callback.dawOnline){
          callback.dawOnline(true);
        }
        sendHostConnectionConfirmation();
        break;
      case 0x10:
        // time code / bbt display
        // 10-digit seven segment display, write multiple digits
        if(callback.dawRecievedLCD){
          callback.dawRecievedLCD(d);
        }
        break;
      case 0x11:
        // 2-digit seven segment display, write multiple digits
        break;
      case 0x12:
        // 56x2 display
        if(callback.dawRecievedLCD){
          callback.dawRecievedLCD(d);
        }
        break;
      case 0x13:
        sendVersionReply();
        break;
      case 0x0A:
        //BMC_PRINTLN("Mackie Transport Button Click", data.sysex[6]);
        break;
      case 0x0B:
        //BMC_PRINTLN("Mackie LCD Backlight saver", data.sysex[6]);
        break;
      case 0x0C:
        //BMC_PRINTLN("Mackie Touchless movable fader", data.sysex[6]);
        break;
      case 0x0E:
        //BMC_PRINTLN("Mackie Fader", data.sysex[6], "touch sensitivity", data.sysex[7]);
        break;
      case 0x0F:
        controller.setOffline();
        if(callback.dawOnline){
          callback.dawOnline(false);
        }
        break;
      case 0x1F:
        // reset?
        break;

      case 0x20:
        {
          //char lcd[5]  = "";
          //char peak[5] = "";
          //char sign[5] = "";

          //strcpy(lcd,  (bitRead(d.sysex[7],2) ? "LCD " : "----"));
          //strcpy(peak, (bitRead(d.sysex[7],1) ? "PEAK" : "----"));
          //strcpy(sign, (bitRead(d.sysex[7],0) ? "SIGN" : "----"));
          //BMC_PRINTLN("Mackie Channel %02x meter mode", d.sysex[6], lcd, peak, sign);
        }
        break;
      case 0x21:
        //BMC_PRINTLN("Mackie Global LCD Meter Mode", d.sysex[6] ? "verical" : "horizontal");
        break;
    }
  }
  bool getLedState(uint8_t cmd, uint8_t ch){
    switch(cmd){
      case BMC_DAW_LED_CMD_REC:                   return controller.getRecState(ch);
      case BMC_DAW_LED_CMD_SOLO:                  return controller.getSoloState(ch);
      case BMC_DAW_LED_CMD_MUTE:                  return controller.getMuteState(ch);
      case BMC_DAW_LED_CMD_SELECT:                return controller.getSelectState(ch);
      case BMC_DAW_LED_CMD_ASSIGN_TRACK:          return controller.getAssignTrack();
      case BMC_DAW_LED_CMD_ASSIGN_PAN:            return controller.getAssignPan();
      case BMC_DAW_LED_CMD_ASSIGN_EQ:             return controller.getAssignEQ();
      case BMC_DAW_LED_CMD_ASSIGN_SEND:           return controller.getAssignSend();
      case BMC_DAW_LED_CMD_ASSIGN_PLUGIN:         return controller.getAssignPlugin();
      case BMC_DAW_LED_CMD_ASSIGN_INSTR:          return controller.getAssignInstr();
      case BMC_DAW_LED_CMD_GLOBAL:                return controller.getFaderViewGlobal();
      case BMC_DAW_LED_CMD_FLIP:                  return controller.getFaderViewFlip();
      case BMC_DAW_LED_CMD_TRANSPORT_PLAY:        return controller.getTransportPlay();
      case BMC_DAW_LED_CMD_TRANSPORT_STOP:        return controller.getTransportStop();
      case BMC_DAW_LED_CMD_TRANSPORT_REC:         return controller.getTransportRecord();
      case BMC_DAW_LED_CMD_TRANSPORT_FORWARD:     return controller.getTransportForward();
      case BMC_DAW_LED_CMD_TRANSPORT_REWIND:      return controller.getTransportRewind();
      case BMC_DAW_LED_CMD_TRANSPORT_MARKER:      return controller.getTransportMarker();
      case BMC_DAW_LED_CMD_TRANSPORT_NUDGE:       return controller.getTransportNudge();
      case BMC_DAW_LED_CMD_TRANSPORT_CYCLE:       return controller.getTransportCycle();
      case BMC_DAW_LED_CMD_TRANSPORT_DROP:        return controller.getTransportDrop();
      case BMC_DAW_LED_CMD_TRANSPORT_REPLACE:     return controller.getTransportReplace();
      case BMC_DAW_LED_CMD_TRANSPORT_CLICK:       return controller.getTransportClick();
      case BMC_DAW_LED_CMD_TRANSPORT_SOLO:        return controller.getTransportSolo();
      case BMC_DAW_LED_CMD_TRANSPORT_ZOOM:        return controller.getTransportCursorZoom();
      case BMC_DAW_LED_CMD_TRANSPORT_SCRUB:       return controller.getTransportScrub();
      case BMC_DAW_LED_CMD_AUTOMATION_READ:       return controller.getAutomationRead();
      case BMC_DAW_LED_CMD_AUTOMATION_WRITE:      return controller.getAutomationWrite();
      case BMC_DAW_LED_CMD_AUTOMATION_TRIM:       return controller.getAutomationTrim();
      case BMC_DAW_LED_CMD_AUTOMATION_TOUCH:      return controller.getAutomationTouch();
      case BMC_DAW_LED_CMD_AUTOMATION_LATCH:      return controller.getAutomationLatch();
      case BMC_DAW_LED_CMD_AUTOMATION_GROUP:      return controller.getAutomationGroup();
      case BMC_DAW_LED_CMD_UTILITY_SAVE:          return controller.getUtilitySave();
      case BMC_DAW_LED_CMD_UTILITY_UNDO:          return controller.getUtilityUndo();
      case BMC_DAW_LED_CMD_METER_PEAK:            return controller.getMeterOverload(ch);
      case BMC_DAW_LED_CMD_METER_1:               return controller.getMeter(ch, 1)>0;
      case BMC_DAW_LED_CMD_METER_2:               return controller.getMeter(ch, 2)>0;
      case BMC_DAW_LED_CMD_METER_3:               return controller.getMeter(ch, 3)>0;
      case BMC_DAW_LED_CMD_METER_4:               return controller.getMeter(ch, 4)>0;
      case BMC_DAW_LED_CMD_METER_5:               return controller.getMeter(ch, 5)>0;
      case BMC_DAW_LED_CMD_METER_6:               return controller.getMeter(ch, 6)>0;
      case BMC_DAW_LED_CMD_METER_7:               return controller.getMeter(ch, 7)>0;
      case BMC_DAW_LED_CMD_METER_8:               return controller.getMeter(ch, 8)>0;
      case BMC_DAW_LED_CMD_METER_9:               return controller.getMeter(ch, 9)>0;
      case BMC_DAW_LED_CMD_METER_10:              return controller.getMeter(ch, 10)>0;
      case BMC_DAW_LED_CMD_METER_11:              return controller.getMeter(ch, 11)>0;
      case BMC_DAW_LED_CMD_METER_12:              return controller.getMeter(ch, 12)>0;
      case BMC_DAW_LED_CMD_VPOT_1:                return controller.getVPotValue(ch, 1)>0;
      case BMC_DAW_LED_CMD_VPOT_2:                return controller.getVPotValue(ch, 2)>0;
      case BMC_DAW_LED_CMD_VPOT_3:                return controller.getVPotValue(ch, 3)>0;
      case BMC_DAW_LED_CMD_VPOT_4:                return controller.getVPotValue(ch, 4)>0;
      case BMC_DAW_LED_CMD_VPOT_5:                return controller.getVPotValue(ch, 5)>0;
      case BMC_DAW_LED_CMD_VPOT_6:                return controller.getVPotValue(ch, 6)>0;
      case BMC_DAW_LED_CMD_VPOT_7:                return controller.getVPotValue(ch, 7)>0;
      case BMC_DAW_LED_CMD_VPOT_8:                return controller.getVPotValue(ch, 8)>0;
      case BMC_DAW_LED_CMD_VPOT_9:                return controller.getVPotValue(ch, 9)>0;
      case BMC_DAW_LED_CMD_VPOT_10:               return controller.getVPotValue(ch, 10)>0;
      case BMC_DAW_LED_CMD_VPOT_11:               return controller.getVPotValue(ch, 11)>0;
      case BMC_DAW_LED_CMD_VPOT_CENTER:           return controller.getVPotCentered(ch);
      case BMC_DAW_LED_CMD_SMPTE:                 return controller.getSMPTE();
      case BMC_DAW_LED_CMD_BEATS:                 return controller.getBeats();
      case BMC_DAW_LED_CMD_RUDE_SOLO:             return controller.getRudeSolo();
      case BMC_DAW_LED_CMD_RELAY:                 return controller.getRelay();
    }
    return false;
  }

  void sendButtonCommand(uint8_t cmd, uint8_t ch, bool release){
    if(ch>=8){
      ch = controller.getSelectedChannel();
    }
    switch(cmd){
      case BMC_DAW_BTN_CMD_REC:     sendNote(0x00+ch, release); break;
      case BMC_DAW_BTN_CMD_SOLO:    sendNote(0x08+ch, release); break;
      case BMC_DAW_BTN_CMD_MUTE:    sendNote(0x10+ch, release); break;
      case BMC_DAW_BTN_CMD_SELECT:  sendNote(0x18+ch, release); break;
      case BMC_DAW_BTN_CMD_VPOT:    sendNote(0x20+ch, release); break;
      case BMC_DAW_BTN_CMD_ASSIGN_TRACK: sendNote(0x28, release); break;
      case BMC_DAW_BTN_CMD_ASSIGN_SEND: sendNote(0x29, release); break;
      case BMC_DAW_BTN_CMD_ASSIGN_PAN: sendNote(0x2A, release); break;
      case BMC_DAW_BTN_CMD_ASSIGN_PLUGIN: sendNote(0x2B, release); break;
      case BMC_DAW_BTN_CMD_ASSIGN_EQ: sendNote(0x2C, release); break;
      case BMC_DAW_BTN_CMD_ASSIGN_INSTR: sendNote(0x2D, release); break;
      case BMC_DAW_BTN_CMD_BANK_LEFT: sendNote(0x2E, release); break;
      case BMC_DAW_BTN_CMD_BANK_RIGHT: sendNote(0x2F, release); break;
      case BMC_DAW_BTN_CMD_CHANNEL_LEFT: sendNote(0x30, release); break;
      case BMC_DAW_BTN_CMD_CHANNEL_RIGHT: sendNote(0x31, release); break;
      case BMC_DAW_BTN_CMD_FLIP: sendNote(0x32, release); break;
      case BMC_DAW_BTN_CMD_GLOBAL: sendNote(0x33, release); break;
      case BMC_DAW_BTN_CMD_NAMEVAL: sendNote(0x34, release); break;
      case BMC_DAW_BTN_CMD_SMPTEBEATS: sendNote(0x35, release); break;
      case BMC_DAW_BTN_CMD_F1: sendNote(0x36, release); break;
      case BMC_DAW_BTN_CMD_F2: sendNote(0x37, release); break;
      case BMC_DAW_BTN_CMD_F3: sendNote(0x38, release); break;
      case BMC_DAW_BTN_CMD_F4: sendNote(0x39, release); break;
      case BMC_DAW_BTN_CMD_F5: sendNote(0x3A, release); break;
      case BMC_DAW_BTN_CMD_F6: sendNote(0x3B, release); break;
      case BMC_DAW_BTN_CMD_F7: sendNote(0x3C, release); break;
      case BMC_DAW_BTN_CMD_F8: sendNote(0x3D, release); break;
      case BMC_DAW_BTN_CMD_VIEW_MIDI: sendNote(0x3E, release); break;
      case BMC_DAW_BTN_CMD_VIEW_INPUTS: sendNote(0x3F, release); break;
      case BMC_DAW_BTN_CMD_VIEW_AUDIO: sendNote(0x40, release); break;
      case BMC_DAW_BTN_CMD_VIEW_INSTR: sendNote(0x41, release); break;
      case BMC_DAW_BTN_CMD_VIEW_AUX: sendNote(0x42, release); break;
      case BMC_DAW_BTN_CMD_VIEW_BUS: sendNote(0x43, release); break;
      case BMC_DAW_BTN_CMD_VIEW_OUT: sendNote(0x44, release); break;
      case BMC_DAW_BTN_CMD_VIEW_USER: sendNote(0x45, release); break;
      case BMC_DAW_BTN_CMD_MOD_SHIFT: sendNote(0x46, release); break;
      case BMC_DAW_BTN_CMD_MOD_OPTION: sendNote(0x47, release); break;
      case BMC_DAW_BTN_CMD_MOD_CONTROL: sendNote(0x48, release); break;
      case BMC_DAW_BTN_CMD_MOD_CMD: sendNote(0x49, release); break;
      case BMC_DAW_BTN_CMD_AUTOMATION_READ: sendNote(0x4A, release); break;
      case BMC_DAW_BTN_CMD_AUTOMATION_WRITE: sendNote(0x4B, release); break;
      case BMC_DAW_BTN_CMD_AUTOMATION_TRIM: sendNote(0x4C, release); break;
      case BMC_DAW_BTN_CMD_AUTOMATION_TOUCH: sendNote(0x4D, release); break;
      case BMC_DAW_BTN_CMD_AUTOMATION_LATCH: sendNote(0x4E, release); break;
      case BMC_DAW_BTN_CMD_AUTOMATION_GROUP: sendNote(0x4F, release); break;
      case BMC_DAW_BTN_CMD_UTILITY_SAVE: sendNote(0x50, release); break;
      case BMC_DAW_BTN_CMD_UTILITY_UNDO: sendNote(0x51, release); break;
      case BMC_DAW_BTN_CMD_UTILITY_CANCEL: sendNote(0x50, release); break;
      case BMC_DAW_BTN_CMD_UTILITY_ENTER: sendNote(0x52, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_MARKER: sendNote(0x54, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_NUDGE: sendNote(0x55, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_CYCLE: sendNote(0x56, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_DROP: sendNote(0x57, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_REPLACE: sendNote(0x58, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_CLICK: sendNote(0x59, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_SOLO: sendNote(0x5A, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_REWIND: sendNote(0x5B, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_FORWARD: sendNote(0x5C, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_STOP: sendNote(0x5D, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_PLAY: sendNote(0x5E, release); break;
      case BMC_DAW_BTN_CMD_TRANSPORT_REC: sendNote(0x5F, release); break;
      case BMC_DAW_BTN_CMD_CURSOR_UP: sendNote(0x60, release); break;
      case BMC_DAW_BTN_CMD_CURSOR_DOWN: sendNote(0x61, release); break;
      case BMC_DAW_BTN_CMD_CURSOR_LEFT: sendNote(0x62, release); break;
      case BMC_DAW_BTN_CMD_CURSOR_RIGHT: sendNote(0x63, release); break;
      case BMC_DAW_BTN_CMD_CURSOR_ZOOM: sendNote(0x64, release); break;
      case BMC_DAW_BTN_CMD_SCRUB: sendNote(0x65, release); break;
      case BMC_DAW_BTN_CMD_FADER_TOUCH:           sendNote(0x68+ch, release); break;
      case BMC_DAW_BTN_CMD_FADER_TOUCH_MASTER:    sendNote(0x70, release); break;



    }
  }

  void sendNote(uint8_t note, bool release=false){
    if(!release){
      midi.sendNoteOn(BMC_USB, 1, note, 127);
    } else {
      midi.sendNoteOff(BMC_USB, 1, note, 0);
    }
  }
  // V-POT
  void sendVPot(uint8_t ch, bool clockwise, uint8_t extraTicks){
    if(ch>=8){
      ch = controller.getSelectedChannel();
    }
    uint8_t value = (clockwise ? 1 : 65)+extraTicks;
    midi.sendControlChange(BMC_USB, 1, 0x10 | ch, value);
  }
  // encoder as fader not yet implemented
  void sendEncoderFader(uint8_t ch, bool clockwise, uint8_t extraTicks){
    ch = (ch>=8) ? controller.getSelectedChannel() : ch;
    int volume = controller.getVolume(ch);
    sendFaderPitch(ch+1, volume, clockwise, extraTicks);
  }
  // encoder as fader not yet implemented
  void sendEncoderMasterFader(bool clockwise, uint8_t extraTicks){
    int volume = controller.getMasterVolume();
    sendFaderPitch(9, volume, clockwise, extraTicks);
  }
  void sendFaderPitch(uint8_t ch, int volume, bool clockwise, uint8_t extraTicks){
    int value = 60*(extraTicks+1);
    if(clockwise){
      if(volume<6651){
        volume = (volume) + value;
      } else {
        return;
      }
      volume = (volume>6651) ? 6651 : volume;
    } else {
      if(volume > (-8192)){
        volume = (volume) - value;
      } else {
        return;
      }
      volume = (volume < (-8192)) ? (-8192) : volume;
    }
    // fader at max/+6db = 14843 / signed 6651
    // fader at unity/0db = 12441 / signed 4249
    // fader at min/infinity = 0 / signed-8192
    midi.sendPitchBend(BMC_USB, ch, volume);
  }


  // scrub wheel
  void sendTransportScrubWheel(bool clockwise, uint8_t extraTicks){
    uint8_t value = (clockwise ? 1 : 65)+extraTicks;
    midi.sendControlChange(BMC_USB, 1, 0x3C, value);
  }
  void sendHostConnectionQuery(){
    //BMC_PRINTLN("sendHostConnectionQuery");
    uint8_t reply[18] =
  	{
  		0xF0, 0x00, 0x00, 0x66, 0x10,      // Prefix
  		0x01,                              // Host Connection Query
  		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, // Serial Number
  		0x4F, 0x4E, 0x45, 0x69,            // Challenge Code
  		0xF7                               // EOX
  	};
    midi.sendSysEx(BMC_USB, reply, 18, true);
  }
  void sendHostConnectionConfirmation(){
    //BMC_PRINTLN("sendHostConnectionConfirmation");
    uint8_t reply[18] =
  	{
  		0xF0, 0x00, 0x00, 0x66, 0x10,      // Prefix
  		0x03,                              // Connection Confirmation
      0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, // Serial Number
  		0x4F, 0x4E, 0x45, 0x69,            // Challenge Code
  		0xF7                               // EOX
  	};
    midi.sendSysEx(BMC_USB, reply, 18, true);
  }
  void sendVersionReply(){
    //BMC_PRINTLN("sendVersionReply");
    uint8_t reply[12] =
    {
      0xF0, 0x00, 0x00, 0x66, 0x10, // Prefix
      0x14,                         // Version Reply
      'V', '2', '.', '5', '3',      // Version
      0xF7,                         // EOX
    };
    midi.sendSysEx(BMC_USB, reply, 12, true);
  }

};

#endif

#endif
