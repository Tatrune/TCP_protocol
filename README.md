# TCP_Transmission<br>
Connect client to server to send files
<br>
open two tab visual studio and run client and server.
<br>

Step 1 (Server): Enter the network port.<br>
Step 2 (Client): Enter the server's IP address.<br>
Step 3 (Client): Enter the network port that matches the port on the server.<br>
Step 4 (Client): Enter the path of the file to be transmitted.<br>
e.g., D:...\file.txt<br><br>
Step 5 (Server): Enter the path where the file will be saved on the server.<br>
e.g., E:...\folder<br><br>
Step 6 (Server): Check on the terminal; if the size of the transmitted file matches total byte received of file, the transmission was successful. If the sizes of file do not match, transmit the file again.<br>
NOTE: The file can only be transmitted if it is smaller than 2GB.<br><br>