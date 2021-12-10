package data;

import java.util.Objects;
import java.io.*;

/**
 * sub class of ClypeData that handles files and their content
 * @author heetdave
 *
 */
public class FileClypeData extends ClypeData {
	private String fileName; //name of file
	private String fileContents; //contents of file
	
	/**
	 * constructor to set up userName, type, fileName, and set fileContents to null
	 * @param userName name of client
	 * @param type data type exchanged
	 * @param fileName name of file
	 */
	public FileClypeData(String userName, String fileName, int type) {
		super(userName, type);
		this.fileName = fileName;
		this.fileContents = null;
	}
	
	/**
	 * default constructor
	 */
	public FileClypeData() {
		this("Anon", "", SEND_A_FILE);
	}
	
	/**
	 * setter method for fileName
	 * @param fileName instantiate name of file
	 */
	public void setFileName(String fileName) {
		this.fileName = fileName;
	}
	
	/**
	 * getter method for fileName
	 * @return name of file
	 */
	public String getFileName() {
		return this.fileName;
	}
	
	/**
	 * getter method for fileContents
	 * @return contents of a file
	 */
	public String getData() {
		return this.fileContents;
	}
	
	/**
	 * getter method for fileContents after decryption
	 * @return decrypted fileContents
	 */
	@Override
	public String getData(String key) {
		return decrypt(fileContents, key);
	}
	
	/**
	 * reads the data in fileName and stores them inside fileContents
	 * throws IO and FileNotFound Exceptions
	 */
	public void readFileContents() {
		try {
			BufferedReader bufferedReader = new BufferedReader(new FileReader(fileName));
			
			String nextLine;
			fileContents = "";
			while((nextLine = bufferedReader.readLine()) != null) {
				fileContents += nextLine;
			}
			
			bufferedReader.close();
		} catch(FileNotFoundException fnfe) {
			System.err.println("File not found " + fnfe.getMessage());
		} catch(IOException ioe) {
			System.err.println("IO Exception occured " + ioe.getMessage());
		}
	}
	
	/**
	 * reads the data in fileName and stores them inside fileContents after encryption
	 * @param key used for encryption
	 * throws IO and FileNotFound Exceptions
	 */
	public void readFileContents(String key) {
		try {
			BufferedReader bufferedReader = new BufferedReader(new FileReader(fileName));
			
			String nextLine;
			fileContents = "";
			while((nextLine = bufferedReader.readLine()) != null) {
				fileContents += nextLine;
			}
			
			bufferedReader.close();
		} catch (FileNotFoundException fnfe) {
			System.err.println("File not found " + fnfe.getMessage());
		} catch (IOException ioe) {
			System.err.println("IO Exception occrued " + ioe.getMessage());	
		}
	}
	
	/**
	 * writes fileContents into fileName
	 * throws IOException
	 */
	public void writeFileContents() {
		try {
			FileWriter fileWriter = new FileWriter(fileName);
			BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
			
			bufferedWriter.write(fileContents);
			bufferedWriter.close();
		} catch (IOException ioe) {
			System.err.println("IO Exception occured " + ioe.getMessage());
		}
	}
	
	/**
	 * writes derypted fileContents into fileName
	 * @param key used for decryption
	 * throws IOException
	 */
	public void writeFileContents(String key) {
		try {
			FileWriter fileWriter = new FileWriter(fileName);
			BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);
			
			decrypt(fileContents, key);
			bufferedWriter.write(fileContents);
			bufferedWriter.close();
		} catch (IOException ioe) {
			System.err.println("IO Exception occured " + ioe.getMessage());	
		}
	}
	
	/**
	 * hashCode() function for all fields in ClypeData and FileClypeData
	 */
	@Override
	public int hashCode() {
		int result = 41;
		result = 47*result + Objects.hashCode(super.getUserName());
		result = 73*result + super.getType();
		result = 23*result + (super.getDate().hashCode());
		result = 11*result + Objects.hashCode(this.fileName);
		result = 29*result + Objects.hashCode(this.fileContents);
		return result;
	}
	 
	/**
	 * equals() comparing objects for fields in hashCode 
	 */
	@Override
	public boolean equals(Object other) {
		if (other == null) return false;
		if (!(other instanceof FileClypeData)) return false;
		FileClypeData otherFileClypeData = (FileClypeData)other;
		return  super.getUserName().equals(otherFileClypeData.getUserName()) &&
				super.getType() == otherFileClypeData.getType() &&
				super.getDate().equals(otherFileClypeData.getDate()) &&
				this.fileName.equals(otherFileClypeData.fileName) &&
				this.fileContents.equals(otherFileClypeData.fileContents);
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
			"fileName: " + this.fileName + "\n" +
			"fileContents: " + this.fileContents + "\n";
	}

	@Override
	public Object getObj() {
		// TODO Auto-generated method stub
		return null;
	}
}
