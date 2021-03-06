package lib;

import java.util.ArrayList;

import android.util.Log;

public class I2C {
	private Board board;
	private I2C.Mode mode;
	
	public enum Mode{
    	MASTER,SLAVE;    	
    }

	public I2C(Board board,I2C.Mode mode,int slaveAddress){
		this.board = board;
		this.mode = mode;
		if (this.mode.equals(I2C.Mode.MASTER)){
			String hexAddress = Integer.toHexString(slaveAddress);
			if (hexAddress.length() < 2) {
			    hexAddress = "0"+hexAddress; // pad with leading zero if needed
			}
			this.board.communicate(Board.Mode.SEND_READ,"CI2CB1M"+hexAddress+"/");
		}
		else if(this.mode.equals(SPI.Mode.SLAVE)){		
		}
	}
	
	public synchronized void send(ArrayList<Integer> data){		
		String hexData="";
		for(int i=0; i<data.size(); i++){
			String hex = Integer.toHexString(data.get(i).intValue());
			if (hex.length() < 2) {
			    hex = "0"+hex; // pad with leading zero if needed
			}
			hexData = hexData + hex;
			
		}
		this.board.communicate(Board.Mode.SEND_READ,"I2CB1T"+hexData+"/");		
	}
	
	public ArrayList<Integer> read(int numBytes){
		String rawList = this.board.communicate(Board.Mode.SEND_READ,"I2CB1R"+Integer.toString(numBytes)+"/");	
		//Log.e("AAA",rawList.toString());
		String[] splitList = rawList.split("\\.");			
		ArrayList<Integer> list = new ArrayList<Integer>();
		
		for(int i=0; i<splitList.length-1; i++){
			
			list.add(Integer.parseInt(splitList[i]));
		}			
		return list;
	}
}
