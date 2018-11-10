app.controller("measurement2Ctrl", function ( $scope, socket ) 
{
    $scope.msg = "Ana Ekran";
    $scope.totalVolume = 0;
    $scope.dailyVolume = 0;
    $scope.lastResetTotalVolume = 0;

    socket.onmessage( function( event ) 
    {
    	//console.log( event.data );
        var data = JSON.parse(event.data);
        
        if( data.messageId != 2 )
        {
            return;
        }
        console.log(data);
        for( var k = 0; k < data.data.length ; k++)
        {
            var dataObj = data.data[k];
            switch( dataObj.id )
            {
                case 20:
                $scope.totalVolume = dataObj.value.toFixed(1); // flow rate
                break;
                case 21:
                var _dt  = new Date(dataObj.value * 1000)
                $scope.lastResetTotalVolume = _dt.getHours().toString()+":"+_dt.getMinutes().toString()+
                                                ":"+_dt.getSeconds().toString()+" "+_dt.getDate().toString()+"/"+
                                                (_dt.getMonth()+1).toString()+"/"+_dt.getFullYear().toString(); // calculated height*/
                //$scope.lastResetTotalVolume = dataObj.value;                                
                break;
                case 22:
                $scope.dailyVolume = dataObj.value.toFixed(1); // temperature
                break;
                default:
                break;
            }
        };
        $scope.$apply();
        //$scope.testVar = 
    });

    $scope.resetTotalVolume = function()
    {
        var dataObj = Object();
        dataObj.appKeyId = 1408;
        dataObj.messageId = 17
        socket.send( JSON.stringify( dataObj ));
    }
});