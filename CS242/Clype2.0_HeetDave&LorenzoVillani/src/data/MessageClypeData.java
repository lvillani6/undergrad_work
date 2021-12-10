package data;

import java.util.Objects;

/**
 * sub class of ClypeData that handles instant messages
 * @author heetdave
 *
 */
public class MessageClypeData extends ClypeData {
	private String message; //instant message

	/**
	 * constructor to set up userName, type, message, and key
	 * @param userName name of client
	 * @param type data type exchanged
	 * @param message instant message
	 * @param key used for encryption
	 */
	public MessageClypeData(String userName, int type, String message, String key) {
		super(userName, type);
		this.message = message;
		encrypt(message, key);
	}
	
	/**
	 * constructor to set up userName, type, and message
	 * @param userName name of client
	 * @param type data type exchanged
	 * @param message instant message
	 */
	public MessageClypeData(String userName, String message, int type) {
		super(userName, type); //super. refers to ClypeData variables
		this.message = message;
	}
	
	/**
	 * default constructor
	 */
	public MessageClypeData() {
		this("Anon", "", SEND_A_MESSAGE);
	}
	
	/**
	 * getter method for message
	 * @return instant message
	 */
	public String getData() {
		return this.message;
	}
	
	/**
	 * getter method for message that decrypts it
	 * @return decrypted message
	 */
	@Override
	public String getData(String key) {
		return decrypt(message, key);
	}
	
	/**
	 * hashCode() function for each field in ClypeData and MessageClypeData
	 */
	@Override
	public int hashCode() {
		int result = 17;
		result = 37*result + Objects.hashCode(super.getUserName());
		result = 31*result + super.getType();
		result = 67*result + Objects.hashCode(this.message);
		result = 61*result + (super.getDate().hashCode());
		return result;
	}
	
	/**
	 * equals() comparing objects for fields in hashCode 
	 */
	@Override
	public boolean equals(Object other) {
		if (other == null) return false;
		if (!(other instanceof MessageClypeData)) return false;
		MessageClypeData otherMessageClypeData = (MessageClypeData)other;
		return  super.getUserName().equals(otherMessageClypeData.getUserName()) &&
				super.getType() == otherMessageClypeData.getType() &&
				super.getDate().equals(otherMessageClypeData.getDate()) &&
				this.message.equals(otherMessageClypeData.message);
	}
	
	/**
	 * toString() returns a description of an object
	 */
	@Override
	public String toString() {
		return 
			"userName: " + super.getUserName() + "\n" +
			"type: " + super.getType() + "\n" +
			"date: " + super.getDate() + "\n" +
			"message: " + this.message + "\n";
	}

	@Override
	public Object getObj() {
		// TODO Auto-generated method stub
		return null;
	}

}
