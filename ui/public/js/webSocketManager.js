var ws = null;
     	
function WebSocketSetup( ) 
{   
  if("WebSocket" in window) 
  {
    // Let us open a web socket
    ws = new WebSocket( "ws://localhost:1233" );

    ws.onopen = function( ) 
    {
      // Web Socket is connected, send data using send()
      var txObj = Object();
      txObj.appKeyId = 1408;
      txObj.messageId = 2;
      txObj.data = [];
      ws.send( JSON.stringify( txObj ) );
      console.log( JSON.stringify( txObj ) );
    };

    ws.onmessage = function( evt ) 
    { 
      var rxObj = JSON.parse( evt.data );
      switch( rxObj.messageId )
      {
    		case 0:
    			
    		break;
    		case 1:
    		break;
    		case 2:
    			setDataChannelValues( rxObj.data );
    			break;
    		case 3:
    		break;
    		default:
  		  break;
      }
    };

    ws.onclose = function() 
    { 
      // websocket is closed.
      // kaantodo: burada gerekli closing ler v.s. yapilmali...
      ws = null;
      console.log("Connection is closed..."); 
    };
  } 
  else 
  {  
    // The browser doesn't support WebSocket
    console.log("WebSocket NOT supported by your Browser!");
  }
}

function WebSocketDisconnect()
{
  if( ws != null )
  {
    if( ws.readyState === WebSocket.OPEN ) 
    {
      ws.close();
      ws = null;
    }
  }
}

function WebSocketConnect()
{
  if( ws == null )
  {
    ws = new WebSocket( "ws://localhost:1233" );
  }
}

function onWebSocketClosed()
{

}

function onWebSocketDataReceived()
{
  
}


