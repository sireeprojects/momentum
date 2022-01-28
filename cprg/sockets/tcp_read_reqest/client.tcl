proc startClient {port {how "-results"}} {
    switch -regexp -- $how {
	"-no?r?e?s?u?l?t?s?" {
	    set sync 0
	} 
	"-re?s?u?l?t?s?" {
	    set sync 1
	}
	default {
	    error "unknown option \"$how\", should be \"-noresults\""
	}
    }
    #client::start "localhost" $port $sync
    client::start "127.0.0.1" $port $sync
    return ""
}

proc client::start {host port waitResult} {
    variable cid
    variable wait
    set wait(wait) $waitResult

    set cid [socket $host $port]
    fconfigure $cid -buffering line -blocking 0
    fileevent $cid readable [namespace code handleData]
    
    #puts "connecting to $host on port $port"
    return ""
}


proc stopClient {} {
    client::stop
    return ""
}

proc request {cmd} {
    client::sendr $cmd
}

namespace eval client {
    variable interp [interp create -safe]
    interp alias $interp response {} [namespace code handleResponse]

    variable cid ""
    variable buffer ""

    variable wait
    array set wait {wait 1 code "ok" result ""}
}

proc client::sendr {cmd} {
    variable cid
    variable wait

    if {$cid == ""} {
	error "client not started"
    }

    puts $cid [list "request" $cmd]
    
# ARUN: do not wait for response
#
#    if {$wait(wait)} {
#	# if we are running asynchronously then wait for a response
#	vwait client::wait(result)
#	return -code $wait(code) $wait(result)
#    }

    return ""
}

proc client::handleResponse {code result} {
    variable cid
    variable wait

    set wait(code) $code
    set wait(result) $result
    return ""
}

proc client::handleData {} {
    variable cid
    variable buffer
    variable interp
    
    if {[gets $cid response] < 0} {
	exit ;# the server disconnected	
    } else {
	append buffer $response "\n"
	if {[info complete $buffer]} {
	    set response $buffer
	    set buffer ""
	    if {[catch {interp eval $interp $response} result]} {
		# the client sent a bad command
		puts stderr "ERROR: server - $result"
	    }
	} else {
	    # partial command, continue waiting for more data	    
	}
    }
    return ""
}

proc client::stop {} {
    variable cid
    close $cid
    set cid ""
    return ""
}

startClient 8888
