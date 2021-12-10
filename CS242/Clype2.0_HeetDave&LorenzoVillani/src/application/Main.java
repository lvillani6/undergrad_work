package application;
	
import data.*;
import main.*;

import java.io.File;
import java.util.ArrayList;
import java.util.Optional;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import javafx.util.Pair;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TextInputDialog;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.scene.media.MediaView;

public class Main extends Application {	
	private ClypeClient client = null;
	private String userName = "anon";
	private String hostName = "localhost";
	private int currentRow = 0;
	private boolean play = false;
	private static final String MEDIA_URL = "file:/C:/Users/lvill/eclipse-workspace/Clype2.0/jar/yodavscountdooku.mp4";
	
	@Override
	public void start(Stage primaryStage) {
		try {
			BorderPane root = new BorderPane();
			root.getStyleClass().add("graytheme");

			GridPane messageLog = new GridPane();
			messageLog.getStyleClass().add("graytheme");
			
			TextArea messageField = new TextArea();
			
			GridPane users = new GridPane();
			users.getStyleClass().add("graytheme");
			
			primaryStage.setOnShown( new EventHandler<WindowEvent>() {
			     public void handle( WindowEvent we ) {
			    	 Dialog<Pair<String, String>> dialog = new Dialog<>();
			    	 dialog.setTitle("New User");
			    	 dialog.setHeaderText(null);
			    	 GridPane grid = new GridPane();
			    	 grid.setHgap(10);
			    	 grid.setVgap(10);
			    	 grid.setPadding(new Insets(20, 150, 10, 10));
			    	 grid.getStyleClass().add("graytheme");
			    	 
			    	 TextField username = new TextField();
			    	 username.setPromptText("Username");
			    	 TextField hostname = new TextField();
			    	 hostname.setPromptText("Hostname");
			    	 
			    	 grid.add(new Label("Username:"), 0, 0);
			    	 grid.add(username, 1, 0);
			    	 grid.add(new Label("Hostname:"), 0, 1);
			    	 grid.add(hostname, 1, 1);
			    	 
			    	 dialog.getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);
			    	 dialog.getDialogPane().setContent(grid);
			    	 
			    	 Platform.runLater(() -> {
			    	        if (!username.isFocused()) {
			    	            username.requestFocus();
			    	        }
			    	    });
			    	 
			    	 dialog.setResultConverter(dialogButton -> {
			    		    if (dialogButton == ButtonType.OK) {
			    		        return new Pair<>(username.getText(), hostname.getText());
			    		    }
			    		    return null;
			    		});
			    	 
			    	 Optional<Pair<String, String>> result = dialog.showAndWait();
			    	 
			    	 result.ifPresent(names -> {
			    		 setUserName(names.getKey());
			    		 setHostName(names.getValue());
			    		 primaryStage.setTitle("Clype 2.0 - " + names.getKey());
			    	});
			    	 
			    	 client = new ClypeClient(userName, hostName);
			    	 client.setMessagePlatform(messageLog, messageField);
			    	 client.setUserPane(users);
			    	 
			    	 System.out.println(client);
			    	 client.start();
			    	 client.sendData(new MessageClypeData(userName, "LISTUSERS", ClypeData.LIST_USERS));
			     }

			});
			
			primaryStage.setOnCloseRequest(new EventHandler<WindowEvent>() {
				@Override
				public void handle(WindowEvent we)	{
					if(!client.getCloseConnection())
						client.sendData(new MessageClypeData(userName, null, ClypeData.LOG_OUT));
				}
			});
			
			//left
			VBox left = new VBox();
			left.getStyleClass().add("graytheme");
			Label leftLabel = new Label();
			leftLabel.setText("Connected Users");
			leftLabel.getStyleClass().add("graytheme");
			left.getChildren().add(leftLabel);
			
			left.getChildren().add(users);
						
			//center
			VBox center = new VBox();
			center.getStyleClass().add("graytheme");
			Label centerLabel = new Label();
			centerLabel.setText("Message Log");
			centerLabel.getStyleClass().add("graytheme");
			center.getChildren().add(centerLabel);
			messageLog.setStyle("-fx-background-color: white");
			
			ScrollPane scroll = new ScrollPane();
			scroll.setContent(messageLog);
			scroll.setFitToWidth(true);
			scroll.setPadding(new Insets(5.0));
			
			center.getChildren().add(scroll);
			//bottom
			HBox bottom = new HBox();
			bottom.getStyleClass().add("graytheme");
			
			VBox buttons = new VBox();
			
			messageField.setWrapText(true);
			
			
			bottom.setAlignment(Pos.CENTER);
			
			Button send = new Button("Send");	
			Button sendFile = new Button("Send File");
			Button sendImage = new Button("Send Image");
			Button sendMedia = new Button("Send Media");
			
			send.setOnAction(new EventHandler<ActionEvent>() {
				@Override
				public void handle(ActionEvent ae)	{
					ObservableList<CharSequence> messageList = messageField.getParagraphs();
					String message = "";
					for(CharSequence c : messageList)	{
						message += c;
					}
					if(!message.equals(""))	{
						logMessage(messageLog, message);
						client.sendData(new MessageClypeData(userName, message, ClypeData.SEND_A_MESSAGE));
					}
					
					messageField.setText("");
				}
			});

			sendFile.setOnAction(new EventHandler<ActionEvent>() {
				@Override
				public void handle(ActionEvent ae)	{
					TextInputDialog dialog = new TextInputDialog();
					dialog.setTitle("Enter file name");
					dialog.setHeaderText("Type the name of the file");
					dialog.setContentText("File name:");

					Optional<String> result = dialog.showAndWait();
					if(result.isPresent())	{
						String fileName = result.get();
						FileClypeData file = new FileClypeData(userName, fileName, ClypeData.SEND_A_FILE);
						file.readFileContents();
						logMessage(messageLog, file.getData());
						client.sendData(file);
					}
				}
			});
			
			sendImage.setOnAction(new EventHandler<ActionEvent>() {
				@Override
				public void handle(ActionEvent ae)	{
					TextInputDialog dialog = new TextInputDialog();
					dialog.setTitle("Enter image name");
					dialog.setHeaderText("Type the name of the image");
					dialog.setContentText("Image name:");
					
					Optional<String> result = dialog.showAndWait();
					if(result.isPresent())	{
						String imageName = result.get();
						SerializableImage image = new SerializableImage();
						image.setImage(new Image(imageName));
						logImage(messageLog, image);
						ImageClypeData cd = new ImageClypeData(userName, image, ClypeData.SEND_AN_IMAGE);
						client.sendData(cd);
					}
				}
			});
			
			sendMedia.setOnAction(new EventHandler<ActionEvent>() {
				@Override
				public void handle(ActionEvent ae)	{
					TextInputDialog dialog = new TextInputDialog();
					dialog.setTitle("Enter media name");
					dialog.setHeaderText("Type the name of the media");
					dialog.setContentText("Media name:");
					
					Optional<String> result = dialog.showAndWait();
					if(result.isPresent())	{
						String mediaName = result.get();
						Media media = new Media("file:/" + mediaName);
						logMedia(messageLog, media);
						client.sendMediaData(mediaName);
					}
				}
			});
			
			messageField.setOnKeyPressed(new EventHandler<KeyEvent>() {
				@Override
				public void handle(KeyEvent ke)	{
					if(ke.getCode() == KeyCode.ENTER)
						send.fire();
				}
			});
			
			messageLog.heightProperty().addListener(new ChangeListener<Number>() 	{
				@Override
				public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue) {
					scroll.setVvalue(scroll.getVmax());					
				}
				
			});
			
			send.setMinSize(20.0, 10.0);
			
			send.getStyleClass().add("greentheme");
			sendFile.getStyleClass().add("greentheme");
			sendImage.getStyleClass().add("greentheme");
			sendMedia.getStyleClass().add("greentheme");
			
			buttons.setMargin(send, new Insets(10.0));
			buttons.setMargin(sendFile, new Insets(10.0));
			buttons.setMargin(sendImage, new Insets(10.0));
			buttons.setMargin(sendMedia, new Insets(10.0));
			
			buttons.getChildren().add(send);
			buttons.getChildren().add(sendFile);
			buttons.getChildren().add(sendImage);
			buttons.getChildren().add(sendMedia);
			bottom.getChildren().add(messageField);
			bottom.getChildren().add(buttons);


			root.setLeft(left);
			root.setBottom(bottom);
			root.setCenter(center);
			
			root.setMargin(left, new Insets(10));
			root.setMargin(bottom, new Insets(10));
			root.setMargin(center, new Insets(10));
			
			Scene scene = new Scene(root,800, 600);
			scene.getStylesheets().add(getClass().getResource("application.css").toExternalForm());
			primaryStage.setScene(scene);
			primaryStage.getIcons().add(new Image("file:C:/Users/lvill/eclipse-workspace/Clype2.0/clypeIcon.png"));
			primaryStage.show();
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	private void setUserName(String userName) {
		this.userName = userName;
	}
	private void setHostName(String hostName) {
		this.hostName = hostName;
	}
	
	private void logMessage(GridPane messageLog, String message)	{
		HBox messageItem = new HBox();
		messageItem.getChildren().add(new Label(message));
		messageItem.setAlignment(Pos.CENTER_RIGHT);
		messageLog.addRow(client.getCurrentRow(), messageItem);
		messageItem.setPrefWidth(messageLog.getWidth());
		client.currentRowInc();
	}
	
	private void logImage(GridPane messageLog, SerializableImage image)	{
		HBox messageItem = new HBox();
		ImageView imageView = new ImageView();
		imageView.setImage(image.getImage());
		imageView.setFitWidth(500.0);
		messageItem.getChildren().add(imageView);
		messageItem.setAlignment(Pos.CENTER_RIGHT);
		messageLog.addRow(client.getCurrentRow(), messageItem);
		messageItem.setPrefWidth(messageLog.getWidth());
		client.currentRowInc();
	}
	
	public void logMedia(GridPane messageLog, Media media)	{
		HBox messageItem = new HBox();
		
        MediaPlayer mediaPlayer = new MediaPlayer(media);
        mediaPlayer.setAutoPlay(false);
        MediaControl mediaControl = new MediaControl(mediaPlayer);
        
		messageItem.getChildren().add(mediaControl);
		messageItem.setAlignment(Pos.CENTER_RIGHT);
		messageLog.addRow(client.getCurrentRow(), messageItem);
		messageItem.setPrefWidth(messageLog.getWidth());
		client.currentRowInc();
	}
//	file:/C:/Users/lvill/eclipse-workspace/Clype2.0/jar/yodavscountdooku.mp4
//	file:/C:/Users/lvill/eclipse-workspace/Clype2.0/jar/
//	C:/Users/lvill/eclipse-workspace/Clype2.0/jar/yodavscountdooku.mp4
	public static void main(String[] args) {
		launch(args);
	}
}
