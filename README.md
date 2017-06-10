# Desc
This is a chatting room by mzy.

# Usage
```
usage: bin/CR --role=string --ip=string --port=short [options] ... 
options:
    -r, --role      CR role, server or client (string)
    --ip        server ip (string)
    --port      server port (short)
    -u, --user      client user name (string [=])
    -d, --daemon    server run as a daemon
    -?, --help      print this message

    CR is a simple chatting room program by mzy 
    CR could run as server or client 
    run as server, you should specify the server ip, port 
    run as client, you should specify the server ip, port and user name 
    client run with a CDK windows, you may input /h for help info
```
# Depend
1. cdk
2. ncurses
3. libevent

# Feature
1. widnow with cdk
2. create room support
3. whisper message support
4. user info





