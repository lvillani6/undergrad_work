package data;

import java.util.Objects;


import javafx.scene.image.Image;
import javafx.scene.media.Media;
import java.io.*;

public class MediaClypeData extends ClypeData {
	private char[] mediaBytes;
	private int numChars = 0;
	
	public MediaClypeData(String userName, int type) {
		super(userName, type); //super. refers to ClypeData variables
		
		mediaBytes = new char[100000000];		//100,000,000
		
	}
	
	public MediaClypeData() {
		this("Anon", ClypeData.SEND_MEDIA);
	}
	
	public Media getObj()	{
		return null;
	}

	public String getData() {
		return "media";
	}
	
	public char[] getMediaBytes()	{
		return this.mediaBytes;
	}
	
	public void setNumChars(int numChars)	{
		this.numChars = numChars;
	}
	
	public int getNumChars()	{
		return this.numChars;
	}
	
	@Override
	public String getData(String key) {
		return "media";
	}
	
	@Override
	public int hashCode() {
		int result = 17;
		result = 37*result + Objects.hashCode(super.getUserName());
		result = 31*result + super.getType();
		result = 61*result + (super.getDate().hashCode());
		return result;
	}
	
	@Override
	public boolean equals(Object other) {
		if (other == null) return false;
		if (!(other instanceof MediaClypeData)) return false;
		MediaClypeData otherMediaClypeData = (MediaClypeData)other;
		return  super.getUserName().equals(otherMediaClypeData.getUserName()) &&
				super.getType() == otherMediaClypeData.getType() &&
				super.getDate().equals(otherMediaClypeData.getDate());
	}
	
	@Override
	public String toString() {
		return 
			"userName: " + super.getUserName() + "\n" +
			"type: " + super.getType() + "\n" +
			"date: " + super.getDate() + "\n";
	}
}
