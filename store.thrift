struct meta {
    1: i32 timestamp
    2: bool success
    3: string ip
    4: i16 port
    5: string result
}

service dkvs {
    meta get (1: i16 key, 2: string consistency)
    meta put (1: i16 key, 2: string value, 3: string consistency, 4: i32 timestamp)
}
