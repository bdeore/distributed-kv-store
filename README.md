## Replicated Key-Value Store with Configurable Consistency

This project is inspired by cassandra distributed key-value store. Project features include configurable consistency
level for writes and reads (quorum or one), hinted-handoff mechanism, write-ahead logs for failure recovery and
replication. key range is between 0 and 255 and is equally split between all nodes. following operations are supported:

``
get `[key]` – given a key, return its corresponding value. put `[key] [value]` – if the key does not already exist, create a new key-value pair; otherwise, update the key to the new value.
``

### build server:

``
make server
``

### start 4 server replicas:

``
./server 1_snitch.txt ./server 2_snitch.txt ./server 3_snitch.txt ./server 4_snitch.txt
``

### build client:

``
make client
``

### run client:

``
./client 1_snitch.txt
``

### commands supported by client:

``
put <key> <value> [-c <one|o|O> | <quorum|q|Q>] [-v]
get <key> [-c <one|o|O> | <quorum|q|Q>] [-v]
select <coordinator node>
``

-c flag specifies desired consistency. -v flag print output in a verbose way.

Note: update snitch files with correct ip addresses. first corresponding entry in the snitch file indicates server's own
ip and port address mapping and is used to initialize the server.

### to rebuild server: clears all write-ahead logs

``
make restart
``


