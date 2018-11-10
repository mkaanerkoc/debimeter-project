app.factory('socket', [function() {
    var stack = [];
    var onmessageDefer;
    var websocketAddr = window.location.hostname;
    if( websocketAddr == undefined  || websocketAddr == "")
    {
        websocketAddr = "localhost";
    }
    var webSocketUrl = "ws://"+websocketAddr+":1233";
    var socket = {
        ws: new WebSocket( webSocketUrl ),
        send: function( data ) 
        {
            data = JSON.stringify(data);
            if (socket.ws.readyState == 1) {
                socket.ws.send(data);
            } else {
                stack.push(data);
            }
        },
        onmessage: function( callback ) 
        {
            if( socket.ws.readyState == 1 ) 
            {
                socket.ws.onmessage = callback;
            } 
            else 
            {
                onmessageDefer = callback;
            }
        }
    };
    socket.ws.onopen = function(event) {
        for (i in stack) 
        {
            socket.ws.send(stack[i]);
        }
        stack = [];
        if( onmessageDefer ) 
        {
            socket.ws.onmessage = onmessageDefer;
            onmessageDefer = null;
        }
    };
    return socket;
}]);
