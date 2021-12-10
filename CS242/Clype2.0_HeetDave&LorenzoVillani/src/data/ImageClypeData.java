package data;

import java.util.Objects;

import javafx.scene.image.Image;

public class ImageClypeData extends ClypeData {
	private SerializableImage image;

	public ImageClypeData(String userName, SerializableImage image, int type) {
		super(userName, type); //super. refers to ClypeData variables
		this.image = image;
	}
	
	public ImageClypeData() {
		this("Anon", null, ClypeData.SEND_AN_IMAGE);
	}
	
	public SerializableImage getObj()	{
		return this.image;
	}
	
	public void setImage(SerializableImage image)	{
		this.image = image;
	}
	
	public String getData() {
		return "image";
	}
	
	@Override
	public String getData(String key) {
		return "image";
	}
	
	@Override
	public int hashCode() {
		int result = 17;
		result = 37*result + Objects.hashCode(super.getUserName());
		result = 31*result + super.getType();
		result = 67*result + Objects.hashCode(this.image);
		result = 61*result + (super.getDate().hashCode());
		return result;
	}
	
	@Override
	public boolean equals(Object other) {
		if (other == null) return false;
		if (!(other instanceof ImageClypeData)) return false;
		ImageClypeData otherImageClypeData = (ImageClypeData)other;
		return  super.getUserName().equals(otherImageClypeData.getUserName()) &&
				super.getType() == otherImageClypeData.getType() &&
				super.getDate().equals(otherImageClypeData.getDate()) &&
				this.image.equals(otherImageClypeData.image);
	}
	
	@Override
	public String toString() {
		return 
			"userName: " + super.getUserName() + "\n" +
			"type: " + super.getType() + "\n" +
			"date: " + super.getDate() + "\n" +
			"message: " + this.image + "\n";
	}
}
