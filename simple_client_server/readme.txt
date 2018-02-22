/****************************************************************************************
S I M P L E  C L I E N T  S E R V E R
****************************************************************************************/
A simple concurrent connection-oriented server that can do something reasonably simple for connected clients. It should be able to carry out such processing for the client as many times as the client wants until the client indicates it wishes to end the session.

The server processes strings and sends the result back to the client. The server will process as many strings as the client signals it wishes to end the session with the use of the ‘&’ character.

Server and client are run in seperate terminals
Server usage: run first, no args, port no. is hard-coded
client usage: <local host> <port-no>

/****************************************************************************************
Output Example
****************************************************************************************/

/** SERVER  **/

H: Server is initialising...
H:	Rendezvous socket descriptor = 3

H:	Socket has been bound successfully: it now has a
H:	local address so that other sockets may connect to it

H:	Socket has been marked as a passive and is listening
H:	for incoming connection requests...
/* client starts up */
H:	Accepted a connection from localhost on port 50260 with empheral sd:4

H:	Process with pid 1114 will service this client
/* Client sends */
	Child with pid 1114: starting up
	Child with pid 1114: Have read in:
	Child with pid 1114: 84	T
	Child with pid 1114: 104	h
	Child with pid 1114: 105	i
	Child with pid 1114: 115	s
	Child with pid 1114: 32	 
	Child with pid 1114: 105	i
	Child with pid 1114: 115	s
	Child with pid 1114: 32	 
	Child with pid 1114: 97	a
	Child with pid 1114: 32	 
	Child with pid 1114: 115	s
	Child with pid 1114: 116	t
	Child with pid 1114: 114	r
	Child with pid 1114: 105	i
	Child with pid 1114: 110	n
	Child with pid 1114: 103	g
	Child with pid 1114: 10	

	Child with pid 1114: Have read in:
	Child with pid 1114: 97	a
	Child with pid 1114: 110	n
	Child with pid 1114: 111	o
	Child with pid 1114: 116	t
	Child with pid 1114: 104	h
	Child with pid 1114: 101	e
	Child with pid 1114: 114	r
	Child with pid 1114: 32	 
	Child with pid 1114: 115	s
	Child with pid 1114: 116	t
	Child with pid 1114: 114	r
	Child with pid 1114: 105	i
	Child with pid 1114: 110	n
	Child with pid 1114: 103	g
	Child with pid 1114: 10	

	Child with pid 1114: Have read in:
	Child with pid 1114: 84	T
	Child with pid 1114: 104	h
	Child with pid 1114: 101	e
	Child with pid 1114: 32	 
	Child with pid 1114: 102	f
	Child with pid 1114: 105	i
	Child with pid 1114: 110	n
	Child with pid 1114: 97	a
	Child with pid 1114: 108	l
	Child with pid 1114: 32	 
	Child with pid 1114: 115	s
	Child with pid 1114: 116	t
	Child with pid 1114: 114	r
	Child with pid 1114: 105	i
	Child with pid 1114: 110	n
	Child with pid 1114: 103	g
	Child with pid 1114: 10	
/* Client request, & to terminate string */
	Child with pid 1114: Have read in:
	Child with pid 1114: 38	&
	Child with pid 1114: 10	
/* Client signals to finish session */
Child with pid 1114: Client has ended session, terminating...
H:	Child pid 1114 has had its exit status accounted for


/** CLIENT  **/

LMBP:DSP_Git lukenewman$ ./yo localhost 7858
<server msg>
Connected to Prac Server on host LMBP.local
Enter X as the first character to exit. 
Otherwise enter the string to be case toggled.
<end server msg>
This is a string
another string
The final string 
&
<server msg>
The server received 50 characters, which when the case is toggled are:
tHIS IS A STRING
ANOTHER STRING
tHE FINAL STRING
&

Enter next string: 
<end server msg>
X&
Client has terminated successfully
LMBP:17807858-set-2 lukenewman$ 

