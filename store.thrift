struct meta {
    1: i32 timestamp
    2: bool success
    3: string ip
    4: i16 port
}

service dkvs {
    string get (1: i16 key, 2: string consistency)
    meta put (1: i16 key, 2: string value, 3: string consistency)
}