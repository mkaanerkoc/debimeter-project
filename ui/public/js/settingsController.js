setTimeout( function()
{
    document.getElementsByTagName('INPUT')[0].focus();
}, 15000);

app.controller("settingsCtrl", function ( $scope, socket ) 
{
	$scope.msg = "Ayarlar Menusu";

	$scope.ipAddress = "";
	$scope.screenSaverPeriod = 30;


	$scope.saveSettings = function()
	{
		var dataObj = Object();
		dataObj.appKeyId = 1408;
		dataObj.messageId = 3;
		dataObj.ipAddress = "192.168.0.10";
		dataObj.screenSaverPeriod = 30;
		socket.send( JSON.stringify( dataObj ));
	}

	socket.onmessage( function( event ) 
    {
        var data = JSON.parse(event.data);
        
        if( data.messageId != 3 )
        {
            return;
        }
         
        $scope.$apply();
    });

    $scope.rows = [
            ['q','w','e','r','t','y','u','i','o','p'],
            ['a','s','d','f','g','h','j','k','l', {type: 'erase', text: '&lArr;'}],
            [{type: 'shift', upperCase: '&dArr;', lowerCase: '&uArr;'}, 'z','x','c','v','b','n','m',',', '.'],
            [{type: 'space', colspan: 10, text: ' '}]
    ];
});