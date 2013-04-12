package com.example.btmsp;

import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class Board {
	
	private BluetoothComm communication = null;
	public static final int MESSAGE_READ = 1;
	private MainActivity context=null;	
	
	public Board(MainActivity context, String BTaddress){
		this.context=context;
		communication = new BluetoothComm(this,handler,BTaddress);	
	}
	
	// The Handler that gets information back from the BluetoothChatService
    private final Handler handler = new Handler() {
    	@Override
        public void handleMessage(Message msg) {
           if(msg.what == MESSAGE_READ) {
        	   byte[] readBuf = (byte[]) msg.obj;
               // construct a string from the valid bytes in the buffer
               String message = new String(readBuf, 0, msg.arg1);           	   
           }
    	}
    };
    
    public void send(String data){
    	communication.send(data);
    }
    
    public String read(){
    	return communication.read();
    }

	public void destroy() {
		communication.destroy();		
	}	

	public void noBluetoothSupported() {
		context.finish();		
	}

	public void askEnableBluetooth() {
		Intent enableIntent = new Intent(android.bluetooth.BluetoothAdapter.ACTION_REQUEST_ENABLE);
        context.startActivityForResult(enableIntent, 2);		
	}
	
	public class DigitalOutput{
	      private int pin;
	      private Board board;
	      
	      public DigitalOutput(Board board,int pin){
	    	  this.board=board;
	    	  this.pin=pin;
	    	  board.send("SDO"+pin+"/");
	      }
	      
	      public void write(boolean state){
	    	  if(state == true){
	    		  board.send("DO"+pin+"H/");
	    	  }
	    	  else{
	    		  board.send("DO"+pin+"L/");
	    	  }
	      }
	      
	      public boolean read(){	    	  
	    	  board.send("DO"+pin+"R/");
	    	  if (communication.read().contains("1/")){
	    		  return true;	    		  
	    	  }
	    	  else{
	    		  return false;
	    	  }
	      }
	}
	
	public class DigitalInput{
		Board board;
		int pin;
		
		public DigitalInput(Board board, int pin){
			this.board=board;
			this.pin=pin;
			board.send("SDI"+pin+"/");
		}
		
		public boolean read(){
			board.send("DI"+pin+"/");
			if (communication.read().contains("1/")){
				return true;	    		  
	    	}
	    	else{
	    		return false;
	    	}			
		}
	}
	
	public class AnalogInput{
		Board board;
		int pin;
		
		public AnalogInput(Board board, int pin){
			this.board=board;
			this.pin=pin;			
			this.board.send("SAI"+pin+"/");
		}
		
		public int read(){
			board.send("AI"+pin+"/");
			String rawValue = communication.read();
			return Integer.parseInt(rawValue.substring(0, rawValue.length()-1));			
		}
	}
	
	public class PWM{
		Board board;
		int pin;
		int period;
		int duty;
		
		public PWM(Board board, int pin, int period, int duty){
			this.board = board;
			this.pin = pin;
			this.period = period;
			this.duty = duty;
			String periodZeros = "";
			String dutyZeros = "";
						
			
			if (String.valueOf(period).length() == 1){
				dutyZeros = "000";
			}
			else if (String.valueOf(period).length() == 2){
				dutyZeros = "00";
			}
			else if (String.valueOf(period).length() == 3){
				dutyZeros = "0";
			}
			
			if (String.valueOf(duty).length() == 1){
				dutyZeros = "00";
			}
			else if (String.valueOf(duty).length() == 2){
				dutyZeros = "0";
			}
			
			
			this.board.send("SPWM"+pin+periodZeros+period+dutyZeros+duty+"/");
		}	
	}
	
	public DigitalOutput createDigitalOutput(int pin){
		return new DigitalOutput(this,pin);		
	}
	
	public DigitalInput createDigitalInput(int pin){
		return new DigitalInput(this,pin);
	}
	
	public AnalogInput createAnalogInput(int pin){
		return new AnalogInput(this,pin);
	}
	
	public PWM createPWM(int pin, int period, int duty){
		return new PWM(this,pin,period,duty);
	}
	
	
	

	
}


