proc startClient {port} {
    variable cid
    set host "127.0.0.1"
    set cid [socket $host $port]
    fconfigure $cid -buffering line -blocking 0
    fileevent $cid readable [namespace code handleData]
    return ""
}

proc sendr {cmd} {
    variable cid
    #puts $cid [list $cmd]
    puts $cid [list $cmd]
    return ""
}

startClient 8888
