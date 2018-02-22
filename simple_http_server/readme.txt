/****************************************************************************************
S I M P L E  H T T P  S E R V E R
****************************************************************************************/
http://tools.ietf.org/html/rfc2616

An implementation of a very simple HTTP server. The server responds to the HEAD and GET verbs as described below and for all other verbs reply with a 501 error message, and an empty body.

Requests are only allowed for the root document (/), all other documents should report 404 errors.

The root document served up is of type text/pain (to keep things simple) and consists of a random number of lines of the nursery rhyme "Twinkle, Twinkle Little Star". The correct content-length is also reported.

Usage: <server ip> <port_no>


Here are the officially sanctioned lyrics to "Twinkle, Twinkle Little Star".


Twinkle, twinkle, little star,
How I wonder what you are!
Up above the world so high,
Like a diamond in the sky.

When the blazing sun is gone,
When he nothing shines upon,
Then you show your little light,
Twinkle, twinkle, all the night.

Then the traveller in the dark
Thanks you for your tiny sparks;
He could not see which way to go,
If you did not twinkle so.

In the dark blue sky you keep,
And often through my curtains peep,
For you never shut your eye
Till the sun is in the sky.

As your bright and tiny spark
Lights the traveller in the dark,
Though I know not what you are,
Twinkle, twinkle, little star.



/****************************************************************************************
Output Example
(In this example request was made in browser which renders the response in html,
can use telnet, curl, or a custom client).
****************************************************************************************/


Server is initialising, responds to 4 requests then exits
S:	Socket has been bound successfully: it now has a
S:	local address so that other sockets may connect to it
S:	Server Port: 7858
S:	Socket has been marked as a passive and is listening
S:	for incoming connection requests...

S:	Pid 947 will service this client

C:	Child with pid 947: : starting up
C:	Request to child:
GET / HTTP/1.1
Host: 192.168.8.106:7858
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.167 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate
Accept-Language: en-US,en;q=0.9


C:	response:
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 0



C:	Child with pid 947: Response sent, child terminating...

S:	Child pid 947 has had its exit status accounted for

S:	Pid 948 will service this client

C:	Child with pid 948: : starting up
C:	Request to child:
GET / HTTP/1.1
Host: 192.168.8.106:7858
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.167 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate
Accept-Language: en-US,en;q=0.9


C:	response:
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 486

Twinkle, twinkle, little star,
How I wonder what you are!
Up above the world so high
Like a diamond in the sky.
When the blazing sun is gone,
When he nothing shines upon,
Then you show your little light,
Twinkle, twinkle, all the night.
Then the traveller in the dark
Thanks you for your tiny sparks;
He could not see which way to go,
If you did not twinkle so.
In the dark blue sky you keep,
And often through my curtains peep,
For you never shut your eye
Till the sun is in the sky.


C:	Child with pid 948: Response sent, child terminating...

S:	Child pid 948 has had its exit status accounted for

S:	Pid 949 will service this client

C:	Child with pid 949: : starting up
C:	Request to child:
GET / HTTP/1.1
Host: 192.168.8.106:7858
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.167 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate
Accept-Language: en-US,en;q=0.9


C:	response:
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 336

Twinkle, twinkle, little star,
How I wonder what you are!
Up above the world so high
Like a diamond in the sky.
When the blazing sun is gone,
When he nothing shines upon,
Then you show your little light,
Twinkle, twinkle, all the night.
Then the traveller in the dark
Thanks you for your tiny sparks;
He could not see which way to go,


C:	Child with pid 949: Response sent, child terminating...

S:	Child pid 949 has had its exit status accounted for

S:	Pid 950 will service this client

C:	Child with pid 950: : starting up
C:	Request to child:
GET / HTTP/1.1
Host: 192.168.8.106:7858
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.167 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate
Accept-Language: en-US,en;q=0.9


C:	response:
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 582

Twinkle, twinkle, little star,
How I wonder what you are!
Up above the world so high
Like a diamond in the sky.
When the blazing sun is gone,
When he nothing shines upon,
Then you show your little light,
Twinkle, twinkle, all the night.
Then the traveller in the dark
Thanks you for your tiny sparks;
He could not see which way to go,
If you did not twinkle so.
In the dark blue sky you keep,
And often through my curtains peep,
For you never shut your eye
Till the sun is in the sky.
As your bright and tiny spark
Lights the traveller in the dark,
Though I know not what you are,




C:	Child with pid 950: Response sent, child terminating...

S:	Child pid 950 has had its exit status accounted for

#############################################
Reached max responses, server shutting down..
#############################################
LMBP:Desktop lukenewman$ 
