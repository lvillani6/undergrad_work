package main;

/**
 * class to takes over the responsibility of receiving data from the server 
 * and printing it in parallel to the ClypeClient class
 * @author heetdave
 *
 */
public class ClientSideServerListener implements Runnable {
	ClypeClient client = new ClypeClient(); //client object
	
	/**
	 * constructor that instantiates client
	 * @param client
	 */
	public ClientSideServerListener(ClypeClient client) {
		this.client = client;
	}
	
	/**
	 * run method that receives data and prints data
	 */
	@Override
	public void run() {
		while(client.getCloseConnection() == false) {
			client.receiveData();
			client.printData();
		}
	}
}
