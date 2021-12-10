package main;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;

import data.*;

public class ServerSideClientIO implements Runnable	{

	private boolean closeConnection;
	private ClypeData dataToReceiveFromClient;
	private ClypeData dataToSendToClient;
	private ObjectInputStream inFromClient;
	private ObjectOutputStream outToClient;
	private ClypeServer server;
	private Socket clientSocket;
	private String connectedUser;
	
	/**
	 * Constructor to create ServerSideClientIO with initial values of server and client socket and default values of closeConnection, 
	 * dataToReceiveFromClient, dataToSendToClient, inFromClient, and outTo Client.<br>
	 * Sets default values to null.<br>
	 * @param server The server to with the client will connect.
	 * @param clientSocket The socket through which data will be moved.
	 */
	public ServerSideClientIO(ClypeServer server, Socket clientSocket)	{
		this.server = server;
		this.clientSocket = clientSocket;
		this.closeConnection = false;
		this.dataToReceiveFromClient = null;
		this.dataToSendToClient = null;
		this.inFromClient = null;
		this.outToClient = null;
	}
	
	/**
	 * Runs the ServerSideClientIO.
	 */
	@Override
	public void run()	{
		try {
			OutputStream os = clientSocket.getOutputStream();
			InputStream is = clientSocket.getInputStream();
			this.outToClient = new ObjectOutputStream(os);
			this.inFromClient = new ObjectInputStream(is);
			while(!closeConnection)	{
				this.receiveData();
				if(this.dataToReceiveFromClient.getType() != ClypeData.LIST_USERS && this.dataToReceiveFromClient.getType() != ClypeData.LOG_OUT)	{
					if(this.dataToReceiveFromClient.getData() == null)	{
						setUserName(this.dataToReceiveFromClient.getUserName());
					}
					else	{
						this.server.broadcast(this.dataToReceiveFromClient);
					}
				}
				else if(this.dataToReceiveFromClient.getType() == ClypeData.LIST_USERS)	{
					ClypeData currentUsers = new MessageClypeData("hostServer", getUsers(), ClypeData.LIST_USERS);
					this.setDataToSendToClient(currentUsers);
					this.server.broadcast(currentUsers);
				}
				else if(this.dataToReceiveFromClient.getType() == ClypeData.LOG_OUT)	{
					this.server.remove(this);
					
					ClypeData currentUsers = new MessageClypeData("hostServer", getUsers(), ClypeData.LIST_USERS);
					this.setDataToSendToClient(currentUsers);
					this.server.broadcast(currentUsers);
					
					this.closeConnection = true;
					this.server.numberOfUsers();
				}
			}
		} catch (IOException ioe) {
			System.err.println("IO Exception Occurred.");
		}
	}
	
	/**
	 * Sends data to client.
	 */
	public void sendData()	{
		try	{
			if(this.dataToSendToClient != null)
				this.outToClient.writeObject(this.dataToSendToClient);
		} catch	(IOException ioe)	{
			System.err.println("IO Exception Occurred send");
		}
	}
	
	/**
	 * Mutator to set the data to be sent to the client.
	 * @param dataToSendToClient The data to be sent to the client as a ClypeData object.
	 */
	public void setDataToSendToClient(ClypeData dataToSendToClient)	{
		this.dataToSendToClient = dataToSendToClient;
	}
	
	/**
	 * Receives data from client.
	 */
	public void receiveData()	{
		try	{
			System.out.println("pre read object");
			this.dataToReceiveFromClient = (ClypeData) this.inFromClient.readObject();
			System.out.println("post read object");
			System.out.println(dataToReceiveFromClient);
			
			if(this.dataToReceiveFromClient != null)
					System.out.println(this.dataToReceiveFromClient.getData());
		} catch	(ClassNotFoundException cnfe)	{
			System.err.println("Class Not Found");
		} catch	(IOException ioe)	{
			System.err.println("IO Exception Occurred receive" + ioe.getMessage());
			ioe.printStackTrace();
		}
	}
	
	/**
	 * Mutator to set the value of userName.
	 * @param userName The new userName as a String.
	 */
	public void setUserName(String userName)	{
		this.connectedUser = userName;
	}
	
	/**
	 * Accessor for connectedUser.
	 * @return The value of connectedUser as a String.
	 */
	public String getUserName()	{
		return this.connectedUser;
	}
	
	public String getUsers()	{
		ArrayList<String> currentUserList = this.server.getUsers();
		String users = "";
		for(String u : currentUserList)	{
			users += u + "   ";
		}
		return users;
	}
}
