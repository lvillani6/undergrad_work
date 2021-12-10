package main;

import data.*;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ConnectException;
import java.net.NoRouteToHostException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Objects;
import javafx.application.Platform;
import javafx.collections.ObservableList;
import javafx.geometry.HPos;
import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;

import java.util.Scanner;

import application.MediaControl;

/**
 * class that represents the client user
 * @author heetdave
 *
 */
public class ClypeClient {
	public static final int LIST_USERS = 0;
	public static final int LOG_OUT = 1;
	public static final int SEND_A_FILE = 2;
	public static final int SEND_A_MESSAGE = 3;
	public static final int SEND_AN_IMAGE = 4;
	public static final int SEND_MEDIA = 5;
	private static final int DEFAULT_PORT = 7000; //default port
	private String userName; //name of client
	private String hostName; //name of computer representing the server
	private int port; //number server connected to
	private boolean closeConnection; //connection closed or not
	private ClypeData dataToSendToServer; //data sent to server
	private ClypeData dataToReceiveFromServer; //data received from server
	private Scanner inFromStd; //scanner used to read from standard input
	private static final String key = "TIME"; //const key
	private ObjectInputStream inFromServer; //receiving data packets
	private ObjectOutputStream outToServer; //sending data packets
	private Socket skt;
	private GridPane messageLog = null;
	private TextArea messageField = null;
	private int currentRow = 0;
	private GridPane userPane = null;
	
	/**
	 * constructor to set up the userName, hostName, port, closeConnection, 
	 * and ClypeData objects to null
	 * @param userName name of client
	 * @param hostName name of computer representing the server
	 * @param port number server is connected to
	 * throws an IllegalArgumentException if port < 1024 or userName or hostName is null
	 */
	public ClypeClient(String userName, String hostName, int port) throws IllegalArgumentException {
		if(userName.equals(null)) {
			throw new IllegalArgumentException("userName cannot be null");
		}
		this.userName = userName;
		
		if(hostName.equals(null)) {
			throw new IllegalArgumentException("hostName cannot be null");
		}
		this.hostName = hostName;
		
		if(port < 1024) {
			throw new IllegalArgumentException("Port number cannot be less than 1024");
		}
		this.port = port;
		this.closeConnection = false;
		dataToSendToServer = null;
		dataToReceiveFromServer = null;
		inFromServer = null;
		outToServer = null;
	}
	
	/**
	 * constructor to set up userName, hostName, and a default port number
	 * @param userName name of client
	 * @param hostName name of computer representing the server
	 */
	public ClypeClient(String userName, String hostName) {
		this(userName, hostName, DEFAULT_PORT);
	}
	
	/**
	 * constructor to set up localhost as hostName
	 * @param userName name of client
	 */
	public ClypeClient(String userName) {
		this(userName, "localhost");
	}
	
	/**
	 * constructor to set up the userName as anonymous
	 */
	public ClypeClient() {
		this("anon");
	}
	
	/**
	 * starts client communication with the server
	 * socket used to connect to the server
	 */
	public void start() {
		try {
			skt = new Socket(this.hostName, this.port);
			this.outToServer = new ObjectOutputStream(skt.getOutputStream());
			this.inFromServer = new ObjectInputStream(skt.getInputStream());
			
			ClientSideServerListener clientSideServerListener = new ClientSideServerListener(this);
			Thread clientSideServerListenerThread = new Thread(clientSideServerListener);
			
			clientSideServerListenerThread.start();
			
			//sends the user name
			this.dataToSendToServer = new MessageClypeData(this.userName, null, ClypeData.SEND_A_MESSAGE);
			this.sendData();
			
		} catch (UnknownHostException uhe) {
			System.err.println("Unknown host " + uhe.getMessage());
		} catch (NoRouteToHostException nrthe) {
			System.err.println("Server unreachable " + nrthe.getMessage());
		} catch (ConnectException ce) {
			System.err.println("Connection refused " + ce.getMessage());
		} catch (IOException ioe) {
			System.err.println("IO Exception occured " + ioe.getMessage());
		}
	}
	
	/**
	 * reads data from standard input and does various tasks
	 */
	public void readClientData()	{
		String input = this.inFromStd.nextLine();
		Scanner inputParser = new Scanner(input);
		String inputCmd = inputParser.next();
		if(inputCmd.equals("DONE"))	{
			this.dataToSendToServer = new MessageClypeData(this.userName, null, ClypeData.LOG_OUT);
			this.closeConnection = true;
		}
		else if(inputCmd.equals("SENDFILE"))	{
			this.dataToSendToServer = new FileClypeData(this.hostName, inputParser.next(), ClypeData.SEND_A_FILE);
			((FileClypeData) this.dataToSendToServer).readFileContents();
		}
		else if(inputCmd.equals("LISTUSERS"))	{
			this.dataToSendToServer = new MessageClypeData(this.userName, "LISTUSERS", ClypeData.LIST_USERS);
		}
		else	{
			this.dataToSendToServer = new MessageClypeData(this.userName, input, ClypeData.SEND_A_MESSAGE);
		}
		inputParser.close();
	}
	
	/**
	 * sends data to the server
	 */
	public void sendData() {
		try {
			outToServer.writeObject(dataToSendToServer);
		
		} catch (NoRouteToHostException nrthe) {
			System.err.println("Server unreachable " + nrthe.getMessage());
		} catch (ConnectException ce) {
			System.err.println("Connection refused " + ce.getMessage());
		} catch (IOException ioe) {
			System.err.println("IO Exception occured " + ioe.getMessage());
		}
	}
	public void sendData(ClypeData cD) {
		try {
			outToServer.writeObject(cD);
			System.out.println(cD);
		} catch (NoRouteToHostException nrthe) {
			System.err.println("Server unreachable " + nrthe.getMessage());
		} catch (ConnectException ce) {
			System.err.println("Connection refused " + ce.getMessage());
		} catch (IOException ioe) {
			//System.err.println("IO Exception occured " + ioe.getMessage());
		}
	}
	public void sendMediaData(String fileName)	{
		MediaClypeData mCD = null;
		try {
			FileReader f = new FileReader(fileName);
			mCD = new MediaClypeData(this.userName, ClypeData.SEND_MEDIA);
			for(int i = 0; i < 10; ++i)	{
				System.out.print(mCD.getMediaBytes()[i]);
			}
			System.out.println("");
			System.out.println("before read");
			int numChars = f.read(mCD.getMediaBytes());
			mCD.setNumChars(numChars);
			System.out.println("numChars = " + numChars);
			for(int i = 0; i < 10; ++i)	{
				System.out.print(mCD.getMediaBytes()[i]);
			}
			System.out.println("after read");
			f.close();
			outToServer.writeObject(mCD);
//			System.out.println(cD);
		} catch (FileNotFoundException fnfe)	{
			System.err.println("File not found" + fnfe.getMessage());
		} catch (NoRouteToHostException nrthe) {
			System.err.println("Server unreachable " + nrthe.getMessage());
		} catch (ConnectException ce) {
			System.err.println("Connection refused " + ce.getMessage());
		} catch (IOException ioe) {
			System.err.println("IO Exception occured: send media data: " + ioe.getMessage());
		}
	}
	
	/**
	 * gets data from the server
	 */
	public void receiveData() {
		try {
			dataToReceiveFromServer = (ClypeData)inFromServer.readObject();
			
		} catch (NoRouteToHostException nrthe) {
			System.err.println("Server unreachable " + nrthe.getMessage());
		} catch (ConnectException ce) {
			System.err.println("Connection refused " + ce.getMessage());
		} catch (ClassNotFoundException cnfe) {
			System.err.println("Class not found " + cnfe.getMessage());
		} catch (IOException ioe) {
			System.err.println("IO Exception occured " + ioe.getMessage());
		}
	}
	
	/**
	 * prints dataToSendToServer object
	 */
	public void printData() {
		if((dataToReceiveFromServer.getType() == ClypeData.SEND_A_MESSAGE) || (dataToReceiveFromServer.getType() == ClypeData.SEND_A_FILE)) {
			System.out.println(dataToReceiveFromServer.getData());
			String message = dataToReceiveFromServer.getData();
			if((!message.equals("")) && (message != null) && (!this.userName.equals(dataToReceiveFromServer.getUserName())))	{
				Platform.runLater( new Runnable() {
					public void run() {
						logMessage(messageLog, message);
					}
				});
			}
		}
		else if(dataToReceiveFromServer.getType() == ClypeData.LIST_USERS)	{
			Platform.runLater( new Runnable() {
				public void run() {
					userPane.getChildren().clear();
					Scanner users = new Scanner(dataToReceiveFromServer.getData());
					int i = 0;
					while(users.hasNext())	{
						Label userListLabel = new Label();
						userListLabel.getStyleClass().add("graytheme");
						userListLabel.setText(users.next());
						
						userPane.getStyleClass().add("graytheme");
						userPane.addRow(i, userListLabel);
						++i;
					}
				}
			});
		}
		else if((dataToReceiveFromServer.getType() == ClypeData.SEND_AN_IMAGE) && (!this.userName.equals(dataToReceiveFromServer.getUserName())))	{
			System.out.println("image received");
			SerializableImage image = ((ImageClypeData) dataToReceiveFromServer).getObj();
			System.out.print(image);
			Platform.runLater( new Runnable() {
				public void run() {
					logImage(messageLog, image);
				}
			});

		}
		else if((dataToReceiveFromServer.getType() == ClypeData.SEND_MEDIA) && (!this.userName.equals(dataToReceiveFromServer.getUserName())))	{

			try	{
				String fileName = "receiveFile.mp4";	//dataToReceiveFromServer.getFileName();	eventually...
				FileWriter w = new FileWriter(fileName);
				w.write(((MediaClypeData) dataToReceiveFromServer).getMediaBytes(), 0, ((MediaClypeData) dataToReceiveFromServer).getNumChars());
				Media media = new Media("file:/" + fileName);
				w.close();
				Platform.runLater( new Runnable() {
					public void run() {
						logMedia(messageLog, media);
					}
				});
			} catch	(IOException ioe)	{
				System.err.println("IO Exception occurred: media write");
			}
		}
	}
	
	
	public void setUserPane(GridPane gp)	{
		this.userPane = gp;
	}
	
	public void setMessagePlatform(GridPane gp, TextArea ta)	{
		this.messageLog = gp;
		this.messageField = ta;
	}
	
	public void currentRowInc()	{++this.currentRow;}
	
	public int getCurrentRow()	{return this.currentRow;}
	
	private void logMessage(GridPane messageLog, String message)	{
		HBox messageItem = new HBox();
		messageItem.getChildren().add(new Label(message));
		messageItem.setAlignment(Pos.CENTER_LEFT);
		messageLog.addRow(getCurrentRow(), messageItem);
		messageItem.setPrefWidth(messageLog.getWidth());
		++currentRow;
	}
	
	private void logImage(GridPane messageLog, SerializableImage image)	{
		HBox messageItem = new HBox();
		ImageView imageView = new ImageView();
		imageView.setImage(image.getImage());
		imageView.setFitWidth(500.0);
		messageItem.getChildren().add(imageView);
		messageItem.setAlignment(Pos.CENTER_LEFT);
		messageLog.addRow(getCurrentRow(), messageItem);
		messageItem.setPrefWidth(messageLog.getWidth());
		++currentRow;
	}
	
	public void logMedia(GridPane messageLog, Media media)	{
		HBox messageItem = new HBox();
		
        MediaPlayer mediaPlayer = new MediaPlayer(media);
        mediaPlayer.setAutoPlay(false);
        MediaControl mediaControl = new MediaControl(mediaPlayer);
        
		messageItem.getChildren().add(mediaControl);
		messageItem.setAlignment(Pos.CENTER_LEFT);
		messageLog.addRow(getCurrentRow(), messageItem);
		messageItem.setPrefWidth(messageLog.getWidth());
		++currentRow;
	}
	
	/**
	 * getter method for userName
	 * @return name of client
	 */
	public String getUserName() {
		return this.userName;
	}
	
	/**
	 * getter method for hostName
	 * @return name of computer representing the server
	 */
	public String getHostName() {
		return this.hostName;
	}
	
	/**
	 * getter method for port number
	 * @return number on server connected to
	 */
	public int getPort() {
		return this.port;
	}
	
	/**
	 * getter method for closeConnection
	 * @return
	 */
	public boolean getCloseConnection() {
		return this.closeConnection;
	}
	
	/**
	 * hashCode() function for each field in ClypeClient
	 */
	@Override
	public int hashCode() {
		int result = 11;
		result = 31*result + Objects.hashCode(this.userName);
		result = 37*result + Objects.hashCode(this.hostName);
		result = 41*result + port;
		result = 43*result + (closeConnection ? 0 : 1);
		return result;
	}
	
	/**
	 * equals() comparing ClypeClient objects for fields in hashCode()
	 */
	@Override
	public boolean equals(Object other) {
		if (other == null) return false;
		if (!(other instanceof ClypeClient)) return false;
		ClypeClient otherClypeClient = (ClypeClient)other;
		return this.userName.equals(otherClypeClient.userName) &&
				this.hostName.equals(otherClypeClient.hostName) &&
				this.port == otherClypeClient.port &&
				this.closeConnection == otherClypeClient.closeConnection;			
	}

	/**
	 * toString() returns a description for an object
	 */
	@Override
	public String toString() {
		return 
			"userName: " + this.userName + "\n" +
			"hostName: " + this.hostName + "\n" +
			"port: " + this.port + "\n" +
			"closeConnection: " + this.closeConnection + "\n";
	}	
	
	/**
	 * main used for testing ClypeClient cases
	 * @param args command line arguments
	 */
	public static void main(String[] args) {
		Scanner scan;
		
		String remain = "";
		String tempUserName = "";
		String tempHostName = "";
		int tempPort;
		
		if(args.length == 0) {
			ClypeClient casei = new ClypeClient();
			casei.start();
		}
		
		else if(args.length == 1) {
			scan = new Scanner(args[0]);
			scan.useDelimiter("@");
		  
				if(args[0].contains("@")) {
					tempUserName = scan.next();
					remain = scan.next();
			
				  if(remain.contains(":")) {
					String[] str = remain.split(":");
					tempHostName = str[0];
					tempPort = Integer.parseInt(str[1]);
					ClypeClient caseiii = new ClypeClient(tempUserName, tempHostName, tempPort);
					caseiii.start();
		} else {
			tempHostName = remain;
			ClypeClient caseii = new ClypeClient(tempUserName, tempHostName);
		}
		
		} else {
			tempUserName = args[0];
			ClypeClient caseiiii = new ClypeClient(tempUserName);
			caseiiii.start();
		  }
		}
	}
}
