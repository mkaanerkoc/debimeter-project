app.controller("mainCtrl", function ( $scope, socket ) 
{
    $scope.msg = "Ölçüm Ekranı 1";
    $scope.collapseBtnState = 0;
    $scope.collapseBtnLabel = "Show More";

    var vm = this;
    //vm.enableThresholds = { set: 1};
    vm.g0 = 
    {
        title : 'Debi',
        value: 0,
        unit : 'm3/saat',
        min : "1",
        max : "500000",
        foregroundColor: 'rgba(0, 150, 136, 1)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)',
        digit: 0
    };

    vm.g1 = 
    {
        title : 'Seviye',
        value : 0,
        unit: 'mm',
        min : "0",
        max : "6000",
        foregroundColor: 'rgba(0, 150, 136, 1)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)',
        digit: 0
    };

    vm.g2 = 
    {
        title : 'Hız',
        value : 0,
        unit: 'm/s',
        min : "0",
        max : "50",
        foregroundColor: 'rgba(0, 150, 136, 1)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)',
        digit: 2
    };

    vm.g3 = 
    {
        title : 'Basınç',
        unit  : 'Bar',
        value : 0,
        min : "0",
        max : "300",
        foregroundColor: 'rgba(0, 150, 136, 1)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)'
    };

    vm.g4 = 
    {
        title : 'Nem',
        unit: '%',
        value : 0,
        min : "0",
        max : "100",
        foregroundColor: 'rgba( 0, 150, 136, 1)',
        backgroundColor: 'rgba( 0, 0, 0, 0.1)'
    };

    vm.g5 = 
    {
        title : 'AD Milivolt',
        unit : 'mV',
        value : 0,
        min : "0",
        max : "5000",
        foregroundColor: 'rgba(0, 150, 136, 1)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)'
    };

    vm.thresholds = {
      '0': {color: 'green'},
      '8': {color: "orange"},
      '20': {color: 'yellow'},
      '30': {color: 'coral'},
      '80': {color: 'red'}
    }
    vm.empty = {};

    $scope.mainBtnClick = function() 
    {
        console.log("main btn click");
        if( ws != null )
        {
        	var txObj = Object();
			txObj.appKeyId = 1408;
			txObj.messageId = 2;
			txObj.data = [];
			ws.send( JSON.stringify( txObj ) );
        }
    };

    $scope.connectBtnClick = function()
    {
    	WebSocketConnect();
    };

    $scope.disconnectBtnClick = function()
    {
    	WebSocketDisconnect();
    };

    $scope.increaseBtnClick = function()
    {
        vm.g0.value = vm.g0.value + 5;
    };

    $scope.decreaseBtnClick = function()
    {
        vm.g0.value = vm.g0.value - 5;
    };

    $scope.gaugeClick = function( gaugeId )
    {
        console.log(" GaugeClick ID: " + gaugeId.toString());
    };

    $scope.collapseBtnClick = function()
    {
        if( 0 == $scope.collapseBtnState )
        {
            $scope.collapseBtnState = 1;
            $scope.collapseBtnLabel = "Show Less";
        }
        else
        {
            $scope.collapseBtnState = 0;
            $scope.collapseBtnLabel = "Show More";
        }
    };
    
    socket.onmessage( function( event ) 
    {
    	//console.log( event.data );
        var data = JSON.parse(event.data);
        
        if( data.messageId != 2 )
        {
            return;
        }
        //console.log(data);
        for( var k = 0; k < data.data.length ; k++)
        {
            var dataObj = data.data[k];
            switch( dataObj.id )
            {
                case 14:
                vm.g0.value = dataObj.value / 24; // flow rate converted m3/gun to m3/saat
                break;
                case 19:
                vm.g1.value = dataObj.value; // calculated height 
                break;
                case 2:
                vm.g2.value = dataObj.value; // temperature
                break;
                case 10:
                vm.g3.value = dataObj.value; // Pressure
                break;
                case 9:
                vm.g4.value = dataObj.value; // Humidity
                break;
                case 16:
                vm.g5.value = dataObj.value; // ads milivolt
                break;
                default:
                break;
            }
        };
        $scope.$apply();
        //$scope.testVar = 
    });
});

app.config( configApp );

configApp.$inject = ['ngGaugeProvider'];

function configApp( ngGaugeProvider ) {

    // setting the default parameters for
    // gauge instances globally.
   
}



/*

typedef enum dataChannelIdsApp
{
    PHOENIX_VQP = 0,
    PHOENIX_VQPF = 1,
    PHOENIX_VAVG = 2,
    PHOENIX_VRAW = 3,
    PHOENIX_VSN = 4,
    PHOENIX_SNR = 5,
    PHOENIX_AGC = 6,
    PHOENIX_NOT = 7,
    PHOENIX_TEMPERATURE = 8,
    PHOENIX_HUMIDITY = 9,
    PHOENIX_PRESSURE = 10,
    PHOENIX_STD = 11,
    PHOENIX_AMPLITUDE = 12,
    PHOENIX_LEVEL_OUTPUT = 13,
    PHOENIX_FLOW_RATE = 14,
    PHOENIX_LEVEL_INPUT = 15,
    ADS1115_MILIVOLT = 16,
    ADS1115_MILIAMPERE = 17,
    ADS1115_ADC_VALUE = 18,
    ADS1115_CALCULATED_HEIGHT_VALUE = 19
}dataChannelIdsApp;


*/