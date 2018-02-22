/****************************************************************************************
S I M P L E  H T T P  C L I E N T
****************************************************************************************/

Two simple HTTP client that perform a HEAD, and GET request on the root document of a specified web server. HEAD prints out the HTTP response code, Content-type, and Last-Modified fields of the reply.

GET writes the body of the reply to stdout if the HTTP status code is 200, and print just the status code if it isn't. 

All socket and protocol parsing is implemented without the use of any frameworks or libraries.

usage <website>

/****************************************************************************************
Output Example
****************************************************************************************/
/* HEAD */

LMBP:DSP_Git lukenewman$ ./head www.example.com
Connected to Server


Head Request:

HTTP/1.1 200 OK
Content-Type: text/html
Last-Modified: Fri, 09 Aug 2013 23:54:35 GMT


Client has terminated successfully
LMBP:DSP_Git lukenewman$ 



/* GET */

LMBP:DSP_Git lukenewman$ ./get www.example.com
Connected to Server


Get Request:


<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
        
    }
    div {
        width: 600px;
        margin: 5em auto;
        padding: 50px;
        background-color: #fff;
        border-radius: 1em;
    }
    a:link, a:visited {
        color: #38488f;
        text-decoration: none;
    }
    @media (max-width: 700px) {
        body {
            background-color: #fff;
        }
        div {
            width: auto;
            margin: 0 auto;
            border-radius: 0;
            padding: 1em;
        }
    }
    </style>    
</head>

<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is established to be used for illustrative examples in documents. You may use this
    domain in examples without prior coordination or asking for permission.</p>
    <p><a href="http://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>

Client has terminated successfully
LMBP:DSP_Git lukenewman$ 
