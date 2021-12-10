package main;
import java.io.*;
import java.net.*;
import java.util.ArrayList;

import data.*;

/**
 * Class to create ClypeServer object with userName, hostName, and port.
 * 
 * @author Lorenzo Villani
 *
 */

public class ClypeServer {

	private int port;
	private boolean closeConnection;
	private ArrayList<ServerSideClientIO> serverSideClientIOList;
	
	/**
	 * Constructor to create ClypeServer with initial value of port.
	 * 
	 * @param port initial port for ClypeServer
	 */
	public ClypeServer(int port)	throws IllegalArgumentException	{
		if(port < 1024)
			throw new IllegalArgumentException("Invalid port");
		this.port = port;
		this.closeConnection = false;
		this.serverSideClientIOList = new ArrayList<>();
	}
	
	/**
	 * Default constructor to create ClypeServer with default value of port.
	 */
	public ClypeServer()	{
		this(7000);
	}
	
	/**
	 * Starts the ClypeServer
	 */
	public void start()	{
		try {
			ServerSocket sskt = new ServerSocket(this.port);
			
			while(!this.closeConnection)	{
				Socket clientSocket = sskt.accept();
				ServerSideClientIO serverSideClientIO = new ServerSideClientIO(this, clientSocket);
				this.serverSideClientIOList.add(serverSideClientIO);
				Thread serverSideClientIOThread = new Thread(serverSideClientIO);
				serverSideClientIOThread.start();
			}
			sskt.close();
			System.out.println("All Closed");
		} catch (IOException ioe) {
			System.err.println("IO Exception Occurred");
		}
	}
	
	/**
	 * Sends a ClypeData object to all currently connected ClypeClients.
	 * @param dataToBroadcastToClients The ClypeData to be broadcasted.
	 */
	public synchronized void broadcast(ClypeData dataToBroadcastToClients)	{
		for(ServerSideClientIO serverSideClientIO : serverSideClientIOList)	{
			serverSideClientIO.setDataToSendToClient(dataToBroadcastToClients);
			serverSideClientIO.sendData();
		}
	}
	
	/**
	 * Removes the serverSideClientIO from serverSideClientIOList.
	 * @param serverSideClientIO The ServerSideClientIO to be removed.
	 */
	public synchronized void remove(ServerSideClientIO serverSideClientIO)	{
		this.serverSideClientIOList.remove(serverSideClientIO);
		if(serverSideClientIOList.isEmpty())
			this.closeConnection = true;
	}
	
	/**
	 * Gets an ArrayList of currently connected users.
	 * @return The currently connected users as an ArrayList of type String.
	 */
	public ArrayList<String> getUsers()	{
		ArrayList<String> currentUsers = new ArrayList<String>();
		for(ServerSideClientIO serverSideClientIO : serverSideClientIOList)	{
			currentUsers.add(serverSideClientIO.getUserName());
		}
		return currentUsers;
	}
	
	/**
	 * Gets the number of currently connected users.
	 * @return The number of serverSideClientIO objects in the ArrayList serverSideClientIOList as an integer.
	 */
	public int numberOfUsers()	{
		return serverSideClientIOList.size();
	}
	
	/**
	 * Accessor for port.
	 * 
	 * @return value of port as an int
	 */
	public int getPort()	{
		return this.port;
	}
	
	/**
	 * Overridden hashCode() method
	 * 
	 * @return a hash code for object as an int
	 */
	public int hashCode()	{
		int result = 17;
		result = 37 * result + this.port;
		result = 37 * result + (this.closeConnection? 1: 0);
		return result;

	}
	
	/**
	 * Overridden toString() method
	 * 
	 * @return description of the class object as a String
	 */
	public String toString()	{
		return "This is a server." +
				"\nPort: " + this.port +
				"\nConnectionClosed: " + this.closeConnection;

	}
	
	/**
	 * The main function, the starting point for the ClypeServer program.
	 * @param args Optional: of the following form:<br>
	 * &#60;port&#62;<br>
	 */
	public static void main(String[] args)	{
		if(args.length == 0)	{
			ClypeServer srv = new ClypeServer();
			srv.start();
		}
		else if(args.length == 1)	{
			ClypeServer srv = new ClypeServer(Integer.parseInt(args[0]));
			srv.start();
		}
		else
			System.err.println("Command line arguments not valid.");
	}
}
