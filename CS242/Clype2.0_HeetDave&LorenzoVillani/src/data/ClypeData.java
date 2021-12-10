package data;
import java.io.Serializable;
import java.util.Date;

/**
 * Abstract class to create ClypeData object with userName, type, and Date.
 * 
 * @author Lorenzo Villani
 *
 */
public abstract class ClypeData implements Serializable {
	
	private static final long serialVersionUID = 1L;
	public static final int LIST_USERS = 0;
	public static final int LOG_OUT = 1;
	public static final int SEND_A_FILE = 2;
	public static final int SEND_A_MESSAGE = 3;
	public static final int SEND_AN_IMAGE = 4;
	public static final int SEND_MEDIA = 5;
	
	private String userName;
	private int type;
	private Date date;
	
	/**
	 * Constructor to create ClypeData with initial values of userName and type.
	 * 
	 * @param userName	initial userName for ClypeData
	 * @param type	initial type for ClypeData
	 */
	public ClypeData(String userName, int type)	{
		this.userName = userName;
		this.type = type;
		this.date = new Date();
	}
	
	/**
	 * Constructor to create ClypeData with default value of userName and initial value of type.
	 * 
	 * @param type initial type for ClypeData (see getType())
	 */
	public ClypeData(int type)	{
		this("Anon", type);
	}
	
	/**
	 * Default constructor to create ClyeData with default value of userName and type.
	 */
	public ClypeData()	{
		this("Anon", 0);
	}
	 
	/**
	 * Accessor for type.
	 * <br>0: give a listing of all users connected to this session
	 * <br>1: log out, i.e., close this client’s connection
	 * <br>2: send a file
	 * <br>3: send a message
	 * 
	 * @return the value of type as an int
	 */
	public int getType()	{
		return this.type;
	}
	
	/**
	 * Accessor for the userName.
	 * 
	 * @return the contents of userName as a string
	 */
	public String getUserName()	{
		return this.userName;
	}
	
	/**
	 * Accessor for the Date.
	 * 
	 * @return the value of Date as a Date object
	 */
	public Date getDate()	{
		return this.date;
	}
	
	/**
	 * Abstract accessor for Data.
	 * 
	 * @return contents of Data as a String
	 */
	public abstract String getData();
	
	public abstract Object getObj();

	public abstract String getData(String key);
	
	/**
	 * A method to encrypt a string using a vignere cipher.
	 * 
	 * @param inputStringToEncrypt A string to be encrypted
	 * @param key The key used to encrypted
	 * @return The encrypted string
	 */
	protected String encrypt(String inputStringToEncrypt, String key)	{
		int keyLength = key.length();
		String encryptedString = "";
		for(int i = 0, j = 0; i < inputStringToEncrypt.length(); ++i)	{
			char c = inputStringToEncrypt.charAt(i);
			char k = key.charAt(j % keyLength);
			char d = ' ';
			int m = 0;
			int s = 0;
			if(c >= 'A' && c <= 'Z')	{
				m = c - 'A';
				if(k >= 'A' && k <= 'Z')
					s = k - 'A';
				else if(k >= 'a' && k <= 'z')
					s = k - 'a';
				d = (char) (((m + s) % 26) + 'A');	//The actual encryption
				encryptedString += d;	//Add to encrypted String
				++j;
			}
			else if(c >= 'a' && c <= 'z')	{
				m = c - 'a';
				if(k >= 'A' && k <= 'Z')
					s = k - 'A';
				else if(k >= 'a' && k <= 'z')
					s = k - 'a';
				d = (char) (((m + s) % 26) + 'a');	//The actual encryption
				encryptedString += d;	//Add to encrypted String
				++j;
			}
			else
				encryptedString += c;
		}
		return encryptedString;
	}
	
	/**
	 * A method to decrypt a string using a vignere cipher.
	 * 
	 * @param inputStringToDecrypt A string to be decrypted
	 * @param key The key used to dencrypted
	 * @return The decrypted string
	 */
	protected String decrypt(String inputStringToDecrypt, String key)	{
		int keyLength = key.length();
		String decryptedString = "";
		for(int i = 0, j = 0; i < inputStringToDecrypt.length(); ++i)	{
			char c = inputStringToDecrypt.charAt(i);
			char k = key.charAt(j % keyLength);
			char d = ' ';
			int m = 0;
			int s = 0;
			if(c >= 'A' && c <= 'Z')	{
				m = c - 'A';
				if(k >= 'A' && k <= 'Z')
					s = k - 'A';
				else if(k >= 'a' && k <= 'z')
					s = k - 'a';
				d = (char) (((m - s + 26) % 26) + 'A');	//The actual decryption
				decryptedString += d;	//Add to decrypted String
				++j;
			}
			else if(c >= 'a' && c <= 'z')	{
				m = c - 'a';
				if(k >= 'A' && k <= 'Z')
					s = k - 'A';
				else if(k >= 'a' && k <= 'z')
					s = k - 'a';
				d = (char) (((m - s + 26) % 26) + 'a');	//The actual decryption
				decryptedString += d;	//Add to decrypted String
				++j;
			}
			else
				decryptedString += c;
		}
		return decryptedString;

	}
}
