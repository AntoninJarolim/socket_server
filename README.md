# Socket server from scratch
VUT FIT - IPK course  

Server is listening on `localhost` at given port. Returns informations about system on valid requests.  
List of valid request names:
1. `hostname`
2. `cpu-name`
3. `load`
## Usage
### Run
`hinfosvc <port>`  
Run example:  
`./hinfosvc 12345 &`  
Valid URL example:   
`http://localhost:12345/cpu-name`   


## Documentation
Main code of the program is in the `main()` function. It calls function to _get new socket_, _create address for server_, _bind adress to socket_ and _to start comunication_ in this exact order. Next comes never ending while, which repeats this steps:
1. Waiting for new connection
2. Parse received URL
3. Create response
4. Send response
5. Close connection

I've learned programming with sockets from [medium.com article](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa) and I am using code from there.

### Load calculation
Program uses function `char *getLoad()` which calculates cpu load. It uses algorithm explained in [this](https://stackoverflow.com/a/23376195) StackOverflow comment.

### Cpu name function
Program uses function `char *getCpuName()` which gets information from `/proc/cpuinfo` and uses `awk` to filter only cpu name.

### Hostname function
Program uses function `char *getUserName()` to read file `/proc/sys/kernel/hostname` where is the hostname stored.


