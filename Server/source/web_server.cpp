//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Jacob Shea ////////////////////////////////////
////////////////////////////// Local Web Server //////////////////////////////////
// Just run this code and put http://127.0.0.1:50001/ in your browser to access //
//////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>			//terminal output
#include <string>		  //Creation of messages	
#include "Socket.h"
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;

//Communication Port to connect to/from
const int MYPORT = 50001;

//Utility Function for clean exit of program
// @input: message, display to the terminal to prompt for exit
// Closes socket and exits program
int done(const string message) {
	Socket::Cleanup();
	cout << message;
	cin.get();
	exit(0);
}

// READ_CONTENTS - reads through the file and returns a string with the contents 
// Parameters: takes the name of the file in the form of a string 
// Returns: the contents of the file in the form of a string 
string read_contents(string name) {
	// Opens the specified file
	ifstream image (name, ios::in|ios::binary|ios::ate);

	// Determines the size of the file 
	int size = image.tellg();

	// Creates a char pointer to an array of char's with the specified size 
	char * char_contents = new char[size];	

	// Starts the file at the beginning and puts it in the char array 
	image.seekg(0);
	image.read (char_contents, size);

	// Reads the char array into a string to be returned 
	string string_contents;
	for (int i = 0; i < size; i++)
	{
		string_contents += char_contents[i];
	}

	// Returns the contents of the file in the form of a string 
	return string_contents; 
}

// GET_SIZE - finds the size of the respective file 
// Parameters: takes the name of the file as a string
// Returns: the size of the file as a string
string get_size(string name) {

	// Opens the specified file 
	ifstream file(name, ios::binary | ios :: ate);

	// Determines the size of the file
	int size = file.tellg();

	// Converts the int size to a string size 
	string string_size = to_string(size);

	// Closes the file 
	file.close();

	// Returns the size in the form of a string
	return string_size;
}

// HEADER_RESPONSE - creates the header file to be sent to the client
// Parameters: takes the contents, length, and type of the file as strings 
// Returns: the fully formatted header with the contents of the file ready to be sent 
string header_response(string file_contents, string length, string content_type) {
	// Creates a string to put the header information in 
	string header; 

	// Miscellaneous header information 
	header.append("HTTP/1.1 200 OK\r\n");								// The response to the client so that it knows it is ok

	// Gets the current date & time to put on the header 
	time_t cur = time(0);
	char * cur_time = ctime(&cur);
	header.append(cur_time);											// The current time that the header is being sent 

	// Miscellaneous header information cont. 
	header.append("Connection: close\r\n");								// Shows that the connection is made 
	header.append("Content-type: " + content_type +  "\r\n");			// Provides the connection type (either html or image)
	header.append("file size: " + length + "\r\n\r\n");					// Gives  the length of the file being sent 
	header.append(file_contents + "\r\n");								// Has the actual content of the file being sent 
	cout << "The http request being sent: " << endl << header << endl; 

	// Returns a string with all of the specified header information 
	return header;
}

// HTTP_SIM - creates the socket for the client to connect to and responds with the proper header response 
// Parameters: -
// Returns: -
void http_sim() {
	// ---------------------------------- Initialization ----------------------------------------
	//Initilize socket, make sure network stack is available
	if (!Socket::Init()) {
		cerr << "Fail to initialize WinSock!\n";
		//return -1;
	}

	// Step 1: Create a TCP socket
	Socket server("tcp");

	// Step 2: Bind socket to a port (MYPORT)	
	if (!server.sock_bind("", MYPORT)) {
		string str;
		str = "Could not bind to port " + to_string(MYPORT);
		done(str);
	}
		
	// Step 3: Ask my socket to "listen"
	if (!server.sock_listen(1)) 
	{
		done("Could not get socket to listen");
	}
	
	cout << "Server is now listening on port " << MYPORT << endl;
	// Step 4:	Wait to accept a connection from a client.  
	//			The variable conn is the "connected" socket.  This is the socket that is actually connected to the server.
	//			The socket variable server can be used to accept another connection if you want.

	Socket conn = server.sock_accept();

	cout << "A client just connected to me!\n\n";
	// ------------------------------------------ End of Initialization -------------------------


	// ---------------------------------- Http requests -----------------------------------------
	// Waits for and receives get request
	cout << "Waiting for a response... " << endl;
	string msg = conn.msg_recv(); 
	cout << "Client Response: " << endl << msg << endl;

	// Detects if the html is requested by the client
	// Will send the header info with the html data  
	size_t found1 = msg.find("GET / HTTP/1.1"); 
   	if (found1 != string::npos) {						// cite: https://www.geeksforgeeks.org/string-find-in-cpp/
		cout << "-----index.html is requested and will be sent-----" << endl;
		conn.msg_send(header_response(read_contents("assets_index.html"), get_size("assets_index.html"), "text/html; charset = UTF - 8"));
		cout << "-----index.html has been sent-----" << endl;
	}

	// Detects if the Whitworth Logo is requested by the client
	// Will send the header info with the Whitworth Logo data
	size_t found2 = msg.find("GET /WhitworthLogo.jpg HTTP/1.1"); 
   	if (found2 != string::npos) {
		cout << "-----WhitworthLogo.jpg is requested and will be sent-----" << endl;
		conn.msg_send(header_response(read_contents("assets_WhitworthLogo.jpg"), "23,768", "image/gif"));
		cout << "-----WhitworthLogo.jpg has been sent-----" << endl;
	}

	// Detects if the favicon is requested by the client
	// Will send the header info with the favicon data
	size_t found3 = msg.find("GET /favicon.ico HTTP/1.1"); 
   	if (found3 != string::npos) {
		cout << "-----favicon.ico is requested to be sent-----" << endl;
		conn.msg_send(header_response(read_contents("assets_favicon.ico"), "23,768", "image/gif"));
		cout << "-----favicon.ico has been sent-----" << endl;
	}
	cout << endl;
	// ----------------------------- End of http request -----------------------------------------
}

// THE SERVER: serves clients
int main() {

	// While loop will make sure that the server is always available to handle request  
	while(1) {
		http_sim();
	};

	return 0;
}