struct meta {
    1: i32 timestamp
    2: bool success
    3: string ip
    4: i16 port
    5: string result
    6: list<node_info> vc
    7: list<bool> response
}

struct node_info {
    1: string ip
    2: i16 port
}

struct hint {
    1: i16 key
    2: string value
    3: i32 timestamp
}

service dkvs {
    meta get (1: i16 key, 2: string consistency)
    meta put (1: i16 key, 2: string value, 3: string consistency, 4: i32 timestamp, 5:bool is_coordinator)
    void request_handoff(1: node_info n)
    void receive_hint(1: hint h)
}
